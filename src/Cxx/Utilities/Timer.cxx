#include <vtkCommand.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>

class vtkTimerCallback : public vtkCommand
{
public:
  vtkTimerCallback() = default;

  static vtkTimerCallback* New()
  {
    vtkTimerCallback* cb = new vtkTimerCallback;
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
    if (this->TimerCount >= this->maxCount)
    {
      auto iren = dynamic_cast<vtkRenderWindowInteractor*>(caller);
      if (this->timerId > -1)
      {
        iren->DestroyTimer(this->timerId);
      }
    }
  }

private:
  int TimerCount = 0;

public:
  int maxCount = 0;
  int timerId = -1;
};

int main(int, char*[])
{
  // Setup renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Timer");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Initialize must be called prior to creating timer events.
  renderWindow->Render();
  interactor->Initialize();

  // Sign up to receive TimerEvent
  vtkNew<vtkTimerCallback> cb;
  interactor->AddObserver(vtkCommand::TimerEvent, cb);

  int timerId = interactor->CreateRepeatingTimer(100);
  std::cout << "timerId: " << timerId << std::endl;
  // Destroy the timer when maxCount is reached.
  cb->maxCount = 10;
  cb->timerId = timerId;

  // Note: nothing is displayed in the render window.
  // Start the interaction and timer
  interactor->Start();

  return EXIT_SUCCESS;
}
