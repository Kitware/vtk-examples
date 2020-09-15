#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

class vtkTimerCallback2 : public vtkCommand
{
public:
  vtkTimerCallback2() = default;
  ~vtkTimerCallback2() = default;

  int timerId = 0;
  static vtkTimerCallback2* New()
  {
    vtkTimerCallback2* cb = new vtkTimerCallback2;
    cb->TimerCount = 0;
    return cb;
  }
  virtual void Execute(vtkObject* caller, unsigned long eventId,
                       void* vtkNotUsed(callData))
  {
    vtkRenderWindowInteractor* iren =
        dynamic_cast<vtkRenderWindowInteractor*>(caller);
    if (vtkCommand::TimerEvent == eventId)
    {
      ++this->TimerCount;
    }
    if (TimerCount < 36)
    {
      actor->RotateZ(5);
      iren->GetRenderWindow()->Render();
    }
    else
    {
      iren->DestroyTimer();
    }
  }

private:
  int TimerCount = 0;

public:
  vtkActor* actor;
};

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(1.0);
  sphereSource->SetThetaResolution(15);
  sphereSource->SetPhiResolution(15);
  sphereSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->RotateX(90);
  actor->GetProperty()->SetRepresentationToWireframe();

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);

  renderer->SetBackground(colors->GetColor3d("Seashell").GetData());
  // Render and interact
  renderWindow->SetWindowName("RotatingSphere");
  renderWindow->Render();

  // Initialize must be called prior to creating timer events.
  renderWindowInteractor->Initialize();

  // Sign up to receive TimerEvent
  vtkNew<vtkTimerCallback2> cb;
  cb->actor = actor;

  renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, cb);
  int timerId = renderWindowInteractor->CreateRepeatingTimer(100);
  cb->timerId = timerId;

  // Start the interaction and timer
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
