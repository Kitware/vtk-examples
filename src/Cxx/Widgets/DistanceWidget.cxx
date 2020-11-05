#include <vtkDistanceRepresentation.h>
#include <vtkDistanceWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("DistanceWidget");

  renderer->SetBackground(colors->GetColor3d("Navy").GetData());

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkDistanceWidget> distanceWidget;
  distanceWidget->SetInteractor(renderWindowInteractor);
  distanceWidget->CreateDefaultRepresentation();
  static_cast<vtkDistanceRepresentation*>(distanceWidget->GetRepresentation())
      ->SetLabelFormat("%-#6.3g mm");

  // Render an image (lights and cameras are created automatically)
  renderWindow->Render();

  renderWindowInteractor->Initialize();
  renderWindow->Render();
  distanceWidget->On();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
