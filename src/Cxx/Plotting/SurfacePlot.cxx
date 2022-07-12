#include <vtkChartXYZ.h>
#include <vtkContextMouseEvent.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPen.h>
#include <vtkPlotSurface.h>
#include <vtkRegressionTestImage.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVector.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 90220220630ULL
#define VTK_HAS_SETCOLORF 1
#endif

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkChartXYZ> chart;
  chart->SetGeometry(vtkRectf(10.0, 10.0, 630, 470));

  vtkNew<vtkPlotSurface> plot;

  vtkNew<vtkContextView> view;
  view->GetRenderer()->SetBackground(colors->GetColor3d("Silver").GetData());
  view->GetRenderWindow()->SetSize(640, 480);
  view->GetScene()->AddItem(chart);

  // Create a surface
  vtkNew<vtkTable> table;
  vtkIdType numPoints = 70;
  float inc = 9.424778 / (numPoints - 1);
  for (vtkIdType i = 0; i < numPoints; ++i)
  {
    vtkNew<vtkFloatArray> arr;
    table->AddColumn(arr);
  }

  table->SetNumberOfRows(static_cast<vtkIdType>(numPoints));
  for (vtkIdType i = 0; i < numPoints; ++i)
  {
    float x = i * inc;
    for (vtkIdType j = 0; j < numPoints; ++j)
    {
      float y = j * inc;
      table->SetValue(i, j, sin(sqrt(x * x + y * y)));
    }
  }

  // Set up the surface plot we wish to visualize and add it to the chart.
  plot->SetXRange(0, 10.0);
  plot->SetYRange(0, 10.0);
  plot->SetInputData(table);
#if VTK_HAS_SETCOLORF
  plot->GetPen()->SetColorF(colors->GetColor3d("Tomato").GetData());
#else
  plot->GetPen()->SetColorF(colors->GetColor3d("Tomato").GetData());
#endif
  chart->AddPlot(plot);

  view->GetRenderWindow()->SetMultiSamples(0);
  view->GetInteractor()->Initialize();
  view->GetRenderWindow()->SetWindowName("SurfacePlot");
  view->GetRenderWindow()->Render();

  // rotate
  vtkContextMouseEvent mouseEvent;
  mouseEvent.SetInteractor(view->GetInteractor());

  vtkVector2i pos;

  vtkVector2i lastPos;
  mouseEvent.SetButton(vtkContextMouseEvent::LEFT_BUTTON);
  lastPos.Set(100, 50);
  mouseEvent.SetLastScreenPos(lastPos);
  pos.Set(150, 100);
  mouseEvent.SetScreenPos(pos);

  vtkVector2d sP(pos.Cast<double>().GetData());
  vtkVector2d lSP(lastPos.Cast<double>().GetData());
  vtkVector2d screenPos(mouseEvent.GetScreenPos().Cast<double>().GetData());
  vtkVector2d lastScreenPos(
      mouseEvent.GetLastScreenPos().Cast<double>().GetData());

  chart->MouseMoveEvent(mouseEvent);

  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
