#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProgrammableFilter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>

namespace {
void TimerCallbackFunction(vtkObject* caller, long unsigned int eventId,
                           void* clientData, void* callData);

unsigned int counter = 0;
unsigned int maxCount = 6;

void AdjustPoints2(void* arguments)
{
  std::cout << "AdjustPoints2" << std::endl;
  vtkProgrammableFilter* programmableFilter =
      static_cast<vtkProgrammableFilter*>(arguments);

  vtkPoints* inPts = programmableFilter->GetPolyDataInput()->GetPoints();
  vtkIdType numPts = inPts->GetNumberOfPoints();
  vtkNew<vtkPoints> newPts;
  newPts->SetNumberOfPoints(numPts);

  for (vtkIdType i = 0; i < numPts; i++)
  {
    double p[3];
    inPts->GetPoint(i, p);
    newPts->SetPoint(i, p);
  }

  double p0[3];
  inPts->GetPoint(0, p0);
  p0[2] = p0[2] + counter * 0.1;
  newPts->SetPoint(0, p0);

  programmableFilter->GetPolyDataOutput()->CopyStructure(
      programmableFilter->GetPolyDataInput());
  programmableFilter->GetPolyDataOutput()->SetPoints(newPts);
}

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkProgrammableFilter> programmableFilter;
  programmableFilter->SetInputConnection(sphereSource->GetOutputPort());

  programmableFilter->SetExecuteMethod(AdjustPoints2, programmableFilter);

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(programmableFilter->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("DataAnimation");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Initialize must be called prior to creating timer events.
  renderWindowInteractor->Initialize();
  renderWindowInteractor->CreateRepeatingTimer(500);

  vtkNew<vtkCallbackCommand> timerCallback;
  timerCallback->SetCallback(TimerCallbackFunction);
  timerCallback->SetClientData(programmableFilter);

  renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, timerCallback);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MistyRose").GetData());

  // Render and interact
  renderWindow->Render();
  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(2.26841, -1.51874, 1.805);
  camera->SetFocalPoint(-0.148582, 0.0814323, 0.24803);
  camera->SetViewUp(0.157813, 0.800687, 0.577923);
  camera->SetDistance(3.29037);
  camera->SetClippingRange(1.14823, 5.60288);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void TimerCallbackFunction(vtkObject* caller,
                           long unsigned int vtkNotUsed(eventId),
                           void* clientData, void* vtkNotUsed(callData))
{
  cout << "timer callback" << endl;

  auto programmableFilter = static_cast<vtkProgrammableFilter*>(clientData);

  vtkRenderWindowInteractor* iren =
      static_cast<vtkRenderWindowInteractor*>(caller);

  programmableFilter->Modified();

  iren->Render();

  if (counter > maxCount)
  {
    iren->DestroyTimer();
  }

  counter++;
}
} // namespace
