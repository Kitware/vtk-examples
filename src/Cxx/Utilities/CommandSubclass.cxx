#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {

// We wil use these to destroy the timer after maxCounter times.
unsigned int counter = 0;
unsigned int maxCounter = 5;
vtkRenderWindowInteractor* iren;

class CommandSubclass2 : public vtkCommand
{
public:
  vtkTypeMacro(CommandSubclass2, vtkCommand);

  static CommandSubclass2* New()
  {
    return new CommandSubclass2;
  }

  void Execute(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(eventId),
               void* vtkNotUsed(callData))
  {
    std::cout << "timer callback" << std::endl;
    if (counter > maxCounter)
    {
      iren->DestroyTimer();
    }
    counter++;
  }
};

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
  actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("CommandSubclass");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Initialize must be called prior to creating timer events.
  renderWindowInteractor->Initialize();
  iren = renderWindowInteractor;

  vtkNew<CommandSubclass2> timerCallback;
  renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, timerCallback);
  auto timerId = renderWindowInteractor->CreateRepeatingTimer(500);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MistyRose").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
