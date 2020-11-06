#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSplineWidget2.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("SplineWidget");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkSplineWidget2> splineWidget;
  splineWidget->SetInteractor(renderWindowInteractor);

  renderWindowInteractor->Initialize();
  renderWindow->Render();
  splineWidget->On();
  renderer->ResetCamera();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
