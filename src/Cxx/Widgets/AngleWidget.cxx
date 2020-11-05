#include <vtkActor.h>
#include <vtkAngleWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("AngleWidget");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkAngleWidget> angleWidget;
  angleWidget->SetInteractor(renderWindowInteractor);
  angleWidget->CreateDefaultRepresentation();

  // Render
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindow->Render();
  angleWidget->On();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
