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
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->SetWindowName("KeypressObserver");

  vtkNew<vtkCallbackCommand> keypressCallback;
  keypressCallback->SetCallback(KeypressCallbackFunction);
  renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent,
                                      keypressCallback);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

void KeypressCallbackFunction(vtkObject* caller,
                              long unsigned int vtkNotUsed(eventId),
                              void* vtkNotUsed(clientData),
                              void* vtkNotUsed(callData))
{
  std::cout << "Keypress callback" << std::endl;

  vtkRenderWindowInteractor* iren =
      static_cast<vtkRenderWindowInteractor*>(caller);

  std::cout << "Pressed: " << iren->GetKeySym() << std::endl;
}

} // namespace
