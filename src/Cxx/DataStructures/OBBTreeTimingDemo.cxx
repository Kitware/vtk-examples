#include <vtkAxis.h>
#include <vtkBox.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkIdList.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBBTree.h>
#include <vtkPlot.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTable.h>
#include <vtkTimerLog.h>
#include <vtkXMLPolyDataReader.h>

#include <time.h>
#include <vector>

namespace {
void RandomPointInBounds(vtkPolyData* polydata, double p[3]);
void RandomDirection(double v[3]);
void RandomLineThroughVolume(vtkPolyData* polydata, double p1[3], double p2[3]);

double TimeOBBTree(vtkPolyData* polydata, int maxPoints, int numberOfTrials);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> reader;
  reader->SetThetaResolution(30);
  reader->SetPhiResolution(30);
  reader->Update();

  std::cout << "Timing OBBTree..." << std::endl;
  std::vector<std::pair<int, double>> results;
  int numberOfTrials = 100;
  for (int i = 1; i < 20; i++)
  {
    double t = TimeOBBTree(reader->GetOutput(), i, numberOfTrials);
    std::pair<int, double> result(i, t);
    results.push_back(result);
  }

  // Create a table with some points in it
  vtkNew<vtkTable> table;

  vtkNew<vtkFloatArray> maxPointsPerRegion;
  maxPointsPerRegion->SetName("MaxPointsPerRegion");
  table->AddColumn(maxPointsPerRegion);

  vtkNew<vtkFloatArray> runtime;
  runtime->SetName("Run time");
  table->AddColumn(runtime);

  // Fill in the table with some example values
  size_t numPoints = results.size();
  table->SetNumberOfRows(static_cast<vtkIdType>(numPoints));
  for (size_t i = 0; i < numPoints; ++i)
  {
    table->SetValue(static_cast<vtkIdType>(i), 0, results[i].first);
    table->SetValue(static_cast<vtkIdType>(i), 1, results[i].second);
    std::cout << "Put " << results[i].first << " " << results[i].second
              << " in the table." << std::endl;
  }

  // Set up the view
  vtkNew<vtkContextView> view;
  view->GetRenderer()->SetBackground(colors->GetColor3d("White").GetData());
  view->GetRenderWindow()->SetWindowName("OBBTreeTimingDemo");

  // Add multiple line plots, setting the colors etc
  vtkNew<vtkChartXY> chart;
  view->GetScene()->AddItem(chart);
  vtkPlot* line = chart->AddPlot(vtkChart::LINE);
  line->SetInputData(table, 0, 1);
  auto lineColor = colors->HTMLColorToRGBA("Lime").GetData();
  line->SetColor(lineColor[0], lineColor[1], lineColor[2], lineColor[3]);
  line->SetWidth(3.0);
  line->GetXAxis()->SetTitle("Max Points Per Region");
  line->GetYAxis()->SetTitle("Run time");
  // line->GetYAxis()->AutoScale();
  // line->GetYAxis()->SetRange(0,0.02);

  view->GetRenderWindow()->SetMultiSamples(0);

  // Start interactor
  view->GetRenderWindow()->Render();
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();
  return EXIT_SUCCESS;
}

namespace {

void RandomPointInBounds(vtkPolyData* polydata, double p[3])
{
  double bounds[6];
  polydata->GetBounds(bounds);

  double x = bounds[0] + (bounds[1] - bounds[0]) * vtkMath::Random(0.0, 1.0);
  double y = bounds[2] + (bounds[3] - bounds[2]) * vtkMath::Random(0.0, 1.0);
  double z = bounds[4] + (bounds[5] - bounds[4]) * vtkMath::Random(0.0, 1.0);

  p[0] = x;
  p[1] = y;
  p[2] = z;
}

double TimeOBBTree(vtkPolyData* polydata, int maxLevel, int numberOfTrials)
{
  vtkNew<vtkTimerLog> timer;
  timer->StartTimer();

  // this should be changed to time(NULL) to get random behavior
  vtkMath::RandomSeed(0);

  // Create the tree
  vtkNew<vtkOBBTree> obbTree;
  obbTree->SetDataSet(polydata);
  obbTree->AutomaticOff();
  obbTree->SetMaxLevel(maxLevel);
  obbTree->BuildLocator();

  for (int i = 0; i < numberOfTrials; i++)
  {
    double p1[3];
    double p2[3];
    RandomLineThroughVolume(polydata, p1, p2);

    double t;
    double x[3];
    double pcoords[3];
    int subId;
    obbTree->IntersectWithLine(p1, p2, .001, t, x, pcoords, subId);
  }

  timer->StopTimer();

  std::cout << "OBBTree took " << timer->GetElapsedTime() << std::endl;

  return timer->GetElapsedTime();
}

void RandomLineThroughVolume(vtkPolyData* polydata, double p1[3], double p2[3])
{
  double bounds[6];
  polydata->GetBounds(bounds);

  double p[3];
  RandomPointInBounds(polydata, p);

  double v[3];
  RandomDirection(v);

  double lineP1[3];
  double lineP2[3];

  for (unsigned int i = 0; i < 3; i++)
  {
    p1[i] = p[i] + 1000 * v[i];
    p2[i] = p[i] - 1000 * v[i];
  }

  double t1, t2;
  int plane1, plane2;
  vtkBox::IntersectWithLine(bounds, lineP1, lineP2, t1, t2, p1, p2, plane1,
                            plane2);
}

void RandomDirection(double v[3])
{
  v[0] = vtkMath::Random(0.0, 1.0);
  v[1] = vtkMath::Random(0.0, 1.0);
  v[2] = vtkMath::Random(0.0, 1.0);
  vtkMath::Normalize(v);
}

} // namespace
