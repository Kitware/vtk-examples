#include <vtkChartPie.h>
#include <vtkColorSeries.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkIntArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlot.h>
#include <vtkPlotPie.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

namespace {
constexpr int NUM_ITEMS = 5;
constexpr int data[] = {77938, 9109, 2070, 12806, 19514};
// constexpr int data[] = {200,200,200,200,200};
std::string labels[] = {"Books", "New and Popular", "Periodical", "Audiobook",
                        "Video"};
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Set up a 2D scene, add an XY chart to it
  vtkNew<vtkContextView> view;
  view->GetRenderer()->SetBackground(colors->GetColor3d("SlateGray").GetData());
  view->GetRenderWindow()->SetSize(600, 350);
  view->GetRenderWindow()->SetWindowName("PieChart");

  vtkNew<vtkChartPie> chart;
  view->GetScene()->AddItem(chart);

  // Create a table with some points in it...
  vtkNew<vtkTable> table;

  vtkNew<vtkIntArray> arrData;
  vtkNew<vtkStringArray> labelArray;

  arrData->SetName("2008 Circulation");
  for (int i = 0; i < NUM_ITEMS; i++)
  {
    arrData->InsertNextValue(data[i]);
    labelArray->InsertNextValue(labels[i]);
  }

  table->AddColumn(arrData);

  // Create a color series to use with our stacks.
  vtkNew<vtkColorSeries> colorSeries;
  colorSeries->SetColorScheme(vtkColorSeries::WARM);

  // Add multiple line plots, setting the colors etc
  vtkPlotPie* pie = dynamic_cast<vtkPlotPie*>(chart->AddPlot(0));
  pie->SetColorSeries(colorSeries);
  pie->SetInputData(table);
  pie->SetInputArray(0, "2008 Circulation");
  pie->SetLabels(labelArray);

  chart->SetShowLegend(true);

  chart->SetTitle("Circulation 2008");

  // Finally render the scene and compare the image to a reference image
  view->GetRenderWindow()->SetMultiSamples(0);
  view->GetRenderWindow()->Render();
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
