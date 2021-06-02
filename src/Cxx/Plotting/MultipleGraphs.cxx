#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPen.h>
#include <vtkPlot.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>

#include <vector>
#include<array>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

    vtkNew<vtkRenderWindow> renWin;
    renWin->SetWindowName("MultipleGraphs");
    vtkNew<vtkRenderWindowInteractor> iRen;
    iRen->SetRenderWindow(renWin);

    // Setup the viewports
    auto gridDimensionsX = 2;
    auto gridDimensionsY = 1;
    auto rendererSizeX = 320;
    auto rendererSizeY = 240;
    renWin->SetSize(rendererSizeX * gridDimensionsX, gridDimensionsY * gridDimensionsY);

    std::vector<std::array<double, 4>> viewPorts;
    for (auto row = 0; row < gridDimensionsY; ++row)
    {
      for (auto col = 0; col < gridDimensionsX; ++col)
      {
        // index = row * grid_dimensions_x + col

        // (xmin, ymin, xmax, ymax)
        viewPorts.push_back(std::array<double, 4>{
            static_cast<double>(col) / gridDimensionsX,
             static_cast<double>(gridDimensionsY - (row + 1)) / gridDimensionsY,
             static_cast<double>(col + 1) / gridDimensionsX,
             static_cast<double>(gridDimensionsY - row) / gridDimensionsY});
      }
    }


  // Create a table with some points in it.
  vtkNew<vtkTable> table;

  vtkNew<vtkFloatArray> arrX;
  arrX->SetName("X Axis");
  table->AddColumn(arrX);

  vtkNew<vtkFloatArray> arrC;
  arrC->SetName("Cosine");
  table->AddColumn(arrC);

  vtkNew<vtkFloatArray> arrS;
  arrS->SetName("Sine");
  table->AddColumn(arrS);

  // Fill in the table with some example values.
  int numPoints = 40;
  float inc = 7.5 / (numPoints - 1);
  table->SetNumberOfRows(numPoints);
  for (int i = 0; i < numPoints; ++i)
  {
    table->SetValue(i, 0, i * inc);
    table->SetValue(i, 1, cos(i * inc));
    table->SetValue(i, 2, sin(i * inc));
  }

  // Set up the view
  vtkNew<vtkContextView> view;
  view->GetRenderWindow()->SetWindowName("LinePlot");
  view->GetRenderer()->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Add multiple line plots, setting the colors etc
  vtkNew<vtkChartXY> chart;
  view->GetScene()->AddItem(chart);
  vtkPlot* line = chart->AddPlot(vtkChart::LINE);
  line->SetInputData(table, 0, 1);
  line->SetColor(0, 255, 0, 255);
  line->SetWidth(1.0);
  line = chart->AddPlot(vtkChart::LINE);
  line->SetInputData(table, 0, 2);
  line->SetColor(255, 0, 0, 255);
  line->SetWidth(5.0);

  // For dotted line, the line type can be from 2 to 5 for different dash/dot
  // patterns (see enum in vtkPen containing DASH_LINE, value 2):
  // #ifndef WIN32
  //   line->GetPen()->SetLineType(vtkPen::DASH_LINE);
  // #endif
  // (ifdef-ed out on Windows because DASH_LINE does not work on Windows
  //  machines with built-in Intel HD graphics card...)

  // view->GetRenderWindow()->SetMultiSamples(0);

  // Start interactor
  view->GetRenderWindow()->Render();
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
