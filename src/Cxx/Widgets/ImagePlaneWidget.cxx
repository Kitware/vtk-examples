#include <vtkActor.h>
#include <vtkImagePlaneWidget.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkSphereSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkCamera.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ImagePlaneWidget");

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // vtkNew<vtkInteractorStyleTrackballActor> style;
  vtkNew<vtkInteractorStyleTrackballCamera> style;

  renderWindowInteractor->SetInteractorStyle(style);

  vtkNew<vtkImagePlaneWidget> planeWidget;
  planeWidget->SetInteractor(renderWindowInteractor);
  planeWidget->TextureVisibilityOff();

  double origin[3] = {0, 1, 0};
  planeWidget->SetOrigin(origin);
  planeWidget->UpdatePlacement();

  // Render
  renderWindow->Render();

  renderer->GetActiveCamera()->Azimuth(-45);
  renderer->GetActiveCamera()->Zoom(0.85);

  renderWindowInteractor->Initialize();
  renderWindow->Render();
  planeWidget->On();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
