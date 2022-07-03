#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkProgrammableFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {
unsigned int counter = 0;

class CommandSubclass : public vtkCommand
{
public:
  vtkTypeMacro(CommandSubclass, vtkCommand);
  static CommandSubclass* New()
  {
    return new CommandSubclass;
  }

  void Execute(vtkObject* caller, unsigned long vtkNotUsed(eventId),
               void* vtkNotUsed(callData)) override
  {
    std::cout << "timer callback" << std::endl;

    vtkRenderWindowInteractor* iren =
        static_cast<vtkRenderWindowInteractor*>(caller);

    this->ProgrammableFilter->Modified();

    iren->Render();

    if (counter > maxCount)
    {
      iren->DestroyTimer();
    }
  }
  vtkSmartPointer<vtkProgrammableFilter> ProgrammableFilter;

public:
  unsigned int maxCount = 7;
};

void AdjustPoints(void* arguments);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkProgrammableFilter> programmableFilter;
  programmableFilter->SetInputConnection(sphereSource->GetOutputPort());
  programmableFilter->SetExecuteMethod(AdjustPoints, programmableFilter);

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
  renderWindow->SetWindowName("DataAnimationWSubclass");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Initialize must be called prior to creating timer events.
  renderWindowInteractor->Initialize();
  renderWindowInteractor->CreateRepeatingTimer(500);

  vtkNew<CommandSubclass> timerCallback;
  timerCallback->ProgrammableFilter = programmableFilter;

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
void AdjustPoints(void* arguments)
{
  std::cout << "AdjustPoints" << std::endl;
  auto programmableFilter = static_cast<vtkProgrammableFilter*>(arguments);

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
  counter++;
}

} // namespace
