#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
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
class vtkTimerCallback2 : public vtkCommand
{
public:
  vtkTimerCallback2() = default;

  static vtkTimerCallback2* New()
  {
    vtkTimerCallback2* cb = new vtkTimerCallback2;
    cb->TimerCount = 0;
    return cb;
  }

  virtual void Execute(vtkObject* caller, unsigned long eventId,
                       void* vtkNotUsed(callData))
  {
    if (vtkCommand::TimerEvent == eventId)
    {
      ++this->TimerCount;
    }
    std::cout << this->TimerCount << std::endl;
    actor->SetPosition(this->TimerCount, this->TimerCount, 0);
    if (this->TimerCount < this->maxCount)
    {

      vtkRenderWindowInteractor* iren =
          dynamic_cast<vtkRenderWindowInteractor*>(caller);
      iren->GetRenderWindow()->Render();
    }
    else
    {
      vtkRenderWindowInteractor* iren =
          dynamic_cast<vtkRenderWindowInteractor*>(caller);
      if (this->timerId > -1)
      {
        iren->DestroyTimer(this->timerId);
      }
    }
  }

private:
  int TimerCount = 0;

public:
  vtkActor* actor = nullptr;
  int timerId = 0;
  int maxCount = -1;
};
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(2.0);
  sphereSource->SetPhiResolution(30);
  sphereSource->SetThetaResolution(30);

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetSpecular(0.6);
  actor->GetProperty()->SetSpecularPower(30);
  actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Animation");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MistyRose").GetData());

  // Render and interact
  renderWindow->Render();
  renderer->GetActiveCamera()->Zoom(0.8);
  renderWindow->Render();

  // Initialize must be called prior to creating timer events.
  renderWindowInteractor->Initialize();

  // Sign up to receive TimerEvent
  vtkNew<vtkTimerCallback2> cb;
  cb->actor = actor;
  renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, cb);

  int timerId = renderWindowInteractor->CreateRepeatingTimer(500);
  std::cout << "timerId: " << timerId << std::endl;
  // Destroy the timer when maxCount is reached.
  cb->maxCount = 3;
  cb->timerId = timerId;
  // Start the interaction and timer
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
