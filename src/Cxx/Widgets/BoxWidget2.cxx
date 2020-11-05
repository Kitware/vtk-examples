#include <vtkActor.h>
#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget2.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

namespace {
class vtkBoxCallback : public vtkCommand
{
public:
  static vtkBoxCallback* New()
  {
    return new vtkBoxCallback;
  }

  vtkSmartPointer<vtkActor> m_actor;

  void SetActor(vtkSmartPointer<vtkActor> actor)
  {
    m_actor = actor;
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSmartPointer<vtkBoxWidget2> boxWidget =
        dynamic_cast<vtkBoxWidget2*>(caller);

    vtkNew<vtkTransform> t;

    dynamic_cast<vtkBoxRepresentation*>(boxWidget->GetRepresentation())
        ->GetTransform(t);
    this->m_actor->SetUserTransform(t);
  }

  vtkBoxCallback()
  {
  }
};
} // namespace

int main(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkConeSource> coneSource;
  coneSource->SetHeight(1.5);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(coneSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("BurlyWood").GetData());

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Blue").GetData());
  renderer->ResetCamera(); // Reposition camera so the whole scene is visible

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("BoxWidget2");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Use the "trackball camera" interactor style, rather than the default
  // "joystick camera"
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style);

  vtkNew<vtkBoxWidget2> boxWidget;
  boxWidget->SetInteractor(renderWindowInteractor);
  boxWidget->GetRepresentation()->SetPlaceFactor(1); // Default is 0.5
  boxWidget->GetRepresentation()->PlaceWidget(actor->GetBounds());

  // Set up a callback for the interactor to call so we can manipulate the actor
  vtkNew<vtkBoxCallback> boxCallback;
  boxCallback->SetActor(actor);
  boxWidget->AddObserver(vtkCommand::InteractionEvent, boxCallback);

  boxWidget->On();

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
