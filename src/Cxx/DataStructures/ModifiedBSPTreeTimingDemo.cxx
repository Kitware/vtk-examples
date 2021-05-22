#include <vtkAxis.h>
#include <vtkBox.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkIdList.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkModifiedBSPTree.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
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

void RandomPointInBounds(vtkPolyData* polydata, double p[3],
                         vtkMinimalStandardRandomSequence* rng);
void RandomDirection(double v[3], vtkMinimalStandardRandomSequence* rng);
void RandomLineThroughVolume(vtkPolyData* polydata, double p1[3], double p2[3],
                             vtkMinimalStandardRandomSequence* rng);

double TimeModifiedBSPTree(vtkPolyData* polydata, int maxPoints,
                           int numberOfTrials,
                           vtkMinimalStandardRandomSequence* rng);
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> reader;
  reader->SetThetaResolution(30);
  reader->SetPhiResolution(30);
  reader->Update();

  vtkNew<vtkMinimalStandardRandomSequence> rng;
  rng->SetSeed(8775070);
  // rng->SetSeed(0);

  std::cout << "Timing ModifiedBSPTree..." << std::endl;
  std::vector<std::pair<int, double>> results;
  int numberOfTrials = 1000;
  for (int i = 1; i < 20; i++)
  {
    double t = TimeModifiedBSPTree(reader->GetOutput(), i, numberOfTrials, rng);
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
  view->GetRenderWindow()->SetWindowName("ModifiedBSPTreeTimingDemo");

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

  // Start interactor
  view->GetRenderWindow()->Render();
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}

namespace {

void RandomPointInBounds(vtkPolyData* polydata, double p[3],
                         vtkMinimalStandardRandomSequence* rng)
{
  double bounds[6];
  polydata->GetBounds(bounds);

  for (auto i = 0; i < 3; ++i)
  {
    p[i] = bounds[i * 2] +
        (bounds[i * 2 + 1] - bounds[i * 2]) * rng->GetRangeValue(0.0, 1.0);
    rng->Next();
  }
}

double TimeModifiedBSPTree(vtkPolyData* polydata, int maxLevel,
                           int numberOfTrials,
                           vtkMinimalStandardRandomSequence* rng)
{
  vtkNew<vtkTimerLog> timer;
  timer->StartTimer();

  // Create the tree
  vtkNew<vtkModifiedBSPTree> modifiedBSPTree;
  modifiedBSPTree->SetDataSet(polydata);
  modifiedBSPTree->AutomaticOff();
  modifiedBSPTree->SetMaxLevel(maxLevel);
  // modifiedBSPTree->SetNumberOfCellsPerNode(cellsPerNode);
  modifiedBSPTree->BuildLocator();

  for (int i = 0; i < numberOfTrials; i++)
  {
    double p1[3];
    double p2[3];
    RandomLineThroughVolume(polydata, p1, p2, rng);

    double t;
    double x[3];
    double pcoords[3];
    int subId;
    modifiedBSPTree->IntersectWithLine(p1, p2, 0.001, t, x, pcoords, subId);
  }

  timer->StopTimer();

  std::cout << "ModifiedBSPTree took " << timer->GetElapsedTime() << std::endl;

  return timer->GetElapsedTime();
}

void RandomLineThroughVolume(vtkPolyData* polydata, double p1[3], double p2[3],
                             vtkMinimalStandardRandomSequence* rng)
{
  double bounds[6];
  polydata->GetBounds(bounds);

  double p[3];
  RandomPointInBounds(polydata, p, rng);

  double v[3];
  RandomDirection(v, rng);

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

void RandomDirection(double v[3], vtkMinimalStandardRandomSequence* rng)
{
  for (auto i = 0; i < 3; ++i)
  {
    v[i] = rng->GetRangeValue(0.0, 1.0);
    rng->Next();
  }
  vtkMath::Normalize(v);
}
} // namespace
