#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkHoverWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {
class vtkHoverCallback : public vtkCommand
{
public:
  static vtkHoverCallback* New()
  {
    return new vtkHoverCallback;
  }

  vtkHoverCallback()
  {
  }

  virtual void Execute(vtkObject*, unsigned long event,
                       void* vtkNotUsed(calldata))
  {
    switch (event)
    {
    case vtkCommand::TimerEvent:
      std::cout
          << "TimerEvent -> the mouse stopped moving and the widget hovered"
          << std::endl;
      break;
    case vtkCommand::EndInteractionEvent:
      std::cout << "EndInteractionEvent -> the mouse started to move"
                << std::endl;
      break;
    }
  }
};
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("HoverWidget");

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Create the widget
  vtkNew<vtkHoverWidget> hoverWidget;
  hoverWidget->SetInteractor(renderWindowInteractor);
  hoverWidget->SetTimerDuration(1000);

  // Create a callback to listen to the widget's two VTK events
  vtkNew<vtkHoverCallback> hoverCallback;
  hoverWidget->AddObserver(vtkCommand::TimerEvent, hoverCallback);
  hoverWidget->AddObserver(vtkCommand::EndInteractionEvent, hoverCallback);

  renderWindow->Render();

  renderWindowInteractor->Initialize();
  renderWindow->Render();
  hoverWidget->On();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
