#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkDataObject.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkStreamingDemandDrivenPipeline.h>

namespace {
class vtkMyTestFilter : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkMyTestFilter, vtkPolyDataAlgorithm);
  static vtkMyTestFilter* New();

  int RefreshEvent;

  unsigned int Counter;

protected:
  vtkMyTestFilter()
  {
    this->SetNumberOfInputPorts(0);

    this->RefreshEvent = vtkCommand::UserEvent + 1;

    this->Counter = 0;
  }

  int RequestData(vtkInformation*, vtkInformationVector**,
                  vtkInformationVector*)
  {
    this->InvokeEvent(this->RefreshEvent, &this->Counter);
    this->Counter++;
    return 1;
  }

private:
  vtkMyTestFilter(const vtkMyTestFilter&) = delete;
  void operator=(const vtkMyTestFilter&) = delete;
};

vtkStandardNewMacro(vtkMyTestFilter);

void CallbackFunction(vtkObject* caller, long unsigned int eventId,
                      void* clientData, void* callData);

class vtkTimerCallback : public vtkCommand
{
public:
  static vtkTimerCallback* New()
  {
    vtkTimerCallback* cb = new vtkTimerCallback;

    return cb;
  }

  virtual void Execute(vtkObject* caller, unsigned long vtkNotUsed(eventId),
                       void* vtkNotUsed(callData))
  {
    TestFilter->Modified();
    TestFilter->Update();
    auto iren = vtkRenderWindowInteractor::SafeDownCast(caller);
    if (TestFilter->Counter > 10)
    {
      std::cout << "Timer Destroyed: " << iren->DestroyTimer(this->timerId)
                << endl;
    }
  }

  int timerId;

  vtkMyTestFilter* TestFilter;
};
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Add an object to make the scene interesting
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5.0);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Goldenrod").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkMyTestFilter> testFilter;

  vtkNew<vtkCallbackCommand> callback;
  callback->SetCallback(CallbackFunction);
  testFilter->AddObserver(testFilter->RefreshEvent, callback);
  testFilter->Update();

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderWindow->SetWindowName("CallData");
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  // Sign up to receive TimerEvent
  vtkNew<vtkTimerCallback> timerCallback;
  timerCallback->TestFilter = testFilter;

  renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, timerCallback);
  timerCallback->timerId = renderWindowInteractor->CreateRepeatingTimer(100);
  std::cout << "timerId: " << timerCallback->timerId << std::endl;

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void CallbackFunction(vtkObject* vtkNotUsed(caller),
                      long unsigned int vtkNotUsed(eventId),
                      void* vtkNotUsed(clientData), void* callData)
{
  unsigned int* callDataCasted = reinterpret_cast<unsigned int*>(callData);
  std::cout << *callDataCasted << std::endl;
}
} // namespace
