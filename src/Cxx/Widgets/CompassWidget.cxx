#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkCompassRepresentation.h>
#include <vtkCompassWidget.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

class vtkICWValueChangedCallback : public vtkCommand
{
public:
  static vtkICWValueChangedCallback* New()
  {
    return new vtkICWValueChangedCallback();
  }
  virtual void Execute(vtkObject* caller, unsigned long eventId, void* callData)
  {
    vtkCompassWidget* widget = vtkCompassWidget::SafeDownCast(caller);
    vtkCamera* camera = widget->GetCurrentRenderer()->GetActiveCamera();

    // calculate new camera position from compass widget parameters
    double distance = widget->GetDistance();
    double tilt = widget->GetTilt();
    double heading = widget->GetHeading();

    double pos[3];
    pos[0] = distance * cos(vtkMath::RadiansFromDegrees(heading)) *
        cos(vtkMath::RadiansFromDegrees(tilt));
    pos[1] = distance * sin(vtkMath::RadiansFromDegrees(heading)) *
        cos(vtkMath::RadiansFromDegrees(tilt));
    pos[2] = distance * sin(vtkMath::RadiansFromDegrees(tilt));

    camera->SetPosition(pos);
    camera->SetFocalPoint(0, 0, 0);
    camera->SetViewUp(0, 0, 1);
    camera->SetClippingRange(0.1, distance + 1);

    widget->GetCurrentRenderer()->Render();
  }
  vtkICWValueChangedCallback()
  {
  }
};

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkCubeSource> cubeSource;

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(cubeSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("PeachPuff").GetData());

  // a renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  // an interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // create the widget and its representation
  vtkNew<vtkCompassRepresentation> compassRepresentation;
  compassRepresentation->SetMinimumDistance(2);
  compassRepresentation->SetMaximumDistance(10);

  vtkNew<vtkCompassWidget> compassWidget;
  compassWidget->SetInteractor(renderWindowInteractor);
  compassWidget->SetRepresentation(compassRepresentation);
  compassWidget->SetDistance(5.0);

  // create the callback
  vtkNew<vtkICWValueChangedCallback> valueChangedCallback;
  compassWidget->AddObserver(vtkCommand::WidgetValueChangedEvent,
                             valueChangedCallback);

  // add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("CompassWidget");

  renderWindow->Render();
  compassWidget->EnabledOn();

  // no interactor style - camera is moved by widget callback
  renderWindowInteractor->SetInteractorStyle(nullptr);

  // set camera to initial position
  compassWidget->InvokeEvent(vtkCommand::WidgetValueChangedEvent);

  // begin interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
