#include <vtkChartParallelCoordinates.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlot.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Set up a 2D scene, add an XY chart to it
  vtkNew<vtkContextView> view;
  view->GetRenderer()->SetBackground(colors->GetColor3d("SlateGray").GetData());
  view->GetRenderWindow()->SetSize(800, 600);
  view->GetRenderWindow()->SetWindowName("ParallelCoordinates");

  vtkNew<vtkChartParallelCoordinates> chart;
  view->GetScene()->AddItem(chart);

  // Create a table with some points in it...
  vtkNew<vtkTable> table;
  vtkNew<vtkFloatArray> arrX;
  arrX->SetName("Field 1");
  table->AddColumn(arrX);
  vtkNew<vtkFloatArray> arrC;
  arrC->SetName("Field 2");
  table->AddColumn(arrC);
  vtkNew<vtkFloatArray> arrS;
  arrS->SetName("Field 3");
  table->AddColumn(arrS);
  vtkNew<vtkFloatArray> arrS2;
  arrS2->SetName("Field 4");
  table->AddColumn(arrS2);
  // Test charting with a few more points...

  table->SetNumberOfRows(10);
  for (int i = 0; i < 10; ++i)
  {
    table->SetValue(i, 0, 0 * i);
    table->SetValue(i, 1, 1 * i);
    table->SetValue(i, 2, 2 * i);
    table->SetValue(i, 3, 3 * i);
  }

  chart->GetPlot(0)->SetInputData(table);

  view->GetRenderWindow()->SetMultiSamples(0);

  view->GetRenderWindow()->Render();
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
