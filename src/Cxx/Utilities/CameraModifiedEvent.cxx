#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {
void CameraModifiedCallback(vtkObject* caller,
                            long unsigned int vtkNotUsed(eventId),
                            void* vtkNotUsed(clientData),
                            void* vtkNotUsed(callData))
{
  std::cout << caller->GetClassName() << " modified" << std::endl;

  vtkCamera* camera = static_cast<vtkCamera*>(caller);
  // print the interesting stuff
  std::cout << "\tPosition: " << camera->GetPosition()[0] << ", "
            << camera->GetPosition()[1] << ", " << camera->GetPosition()[2]
            << std::endl;
  std::cout << "\tFocal point: " << camera->GetFocalPoint()[0] << ", "
            << camera->GetFocalPoint()[1] << ", " << camera->GetFocalPoint()[2]
            << std::endl;
}
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("CameraModifiedEvent");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkCallbackCommand> modifiedCallback;
  modifiedCallback->SetCallback(CameraModifiedCallback);

  renderer->ResetCamera();
  renderer->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent,
                                           modifiedCallback);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
