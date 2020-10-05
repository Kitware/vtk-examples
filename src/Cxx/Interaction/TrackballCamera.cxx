#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(1.0, 0.0, 0.0);
  sphereSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // Create a cone
  vtkNew<vtkConeSource> coneSource;

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(coneSource->GetOutputPort());

  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(
      colors->GetColor3d("LightGoldenrodYellow").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("TrackballCamera");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(sphereActor);
  renderer->AddActor(coneActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render an image (lights and cameras are created automatically)
  renderWindow->Render();

  vtkNew<vtkInteractorStyleTrackballCamera> style;

  renderWindowInteractor->SetInteractorStyle(style);

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
