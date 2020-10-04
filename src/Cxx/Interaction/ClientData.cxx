#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace {
void KeypressCallbackFunction(vtkObject* caller, long unsigned int eventId,
                              void* clientData, void* callData);
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5.0);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  // Create an actor
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Goldenrod").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkCallbackCommand> keypressCallback;

  // Allow the observer to access the sphereSource
  keypressCallback->SetClientData(sphereSource);
  keypressCallback->SetCallback(KeypressCallbackFunction);
  renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent,
                                      keypressCallback);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderWindow->SetWindowName("ClientData");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void KeypressCallbackFunction(vtkObject*, long unsigned int vtkNotUsed(eventId),
                              void* clientData, void* vtkNotUsed(callData))
{
  // Prove that we can access the sphere source
  auto sphereSource = static_cast<vtkSphereSource*>(clientData);
  std::cout << "Radius is " << sphereSource->GetRadius() << std::endl;
}
} // namespace
