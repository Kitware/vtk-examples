#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTimerLog.h>

namespace {
void CallbackFunction(vtkObject* caller, long unsigned int eventId,
                      void* clientData, void* callData);
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;

  for (unsigned int i = 0; i < 10; i++)
  {
    // Create a sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(i, 0, 0);

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("carrot").GetData());

    renderer->AddActor(actor);
  }

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("FrameRate");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkCallbackCommand> callback;

  callback->SetCallback(CallbackFunction);
  renderer->AddObserver(vtkCommand::EndEvent, callback);
  renderer->SetBackground(colors->GetColor3d("sap_green").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();
  return EXIT_SUCCESS;
}

namespace {

void CallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId),
                      void* vtkNotUsed(clientData), void* vtkNotUsed(callData))
{
  vtkRenderer* renderer = static_cast<vtkRenderer*>(caller);

  double timeInSeconds = renderer->GetLastRenderTimeInSeconds();
  double fps = 1.0 / timeInSeconds;
  std::cout << "FPS: " << fps << std::endl;

  std::cout << "Callback" << std::endl;
}
} // namespace
