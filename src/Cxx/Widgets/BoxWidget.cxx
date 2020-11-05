#include <vtkActor.h>
#include <vtkBoxWidget.h>
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
class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback* New()
  {
    return new vtkMyCallback;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    // Here we use the vtkBoxWidget to transform the underlying coneActor
    // (by manipulating its transformation matrix).
    vtkNew<vtkTransform> t;
    vtkBoxWidget* widget = reinterpret_cast<vtkBoxWidget*>(caller);
    widget->GetTransform(t);
    widget->GetProp3D()->SetUserTransform(t);
  }
};
} // namespace

int main(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkConeSource> cone;

  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(cone->GetOutputPort());

  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(colors->GetColor3d("BurlyWood").GetData());

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(coneActor);
  renderer->SetBackground(colors->GetColor3d("Blue").GetData());

  vtkNew<vtkRenderWindow> window;
  window->AddRenderer(renderer);
  window->SetSize(300, 300);
  window->SetWindowName("BoxWidget");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(window);

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  interactor->SetInteractorStyle(style);

  vtkNew<vtkBoxWidget> boxWidget;
  boxWidget->SetInteractor(interactor);

  boxWidget->SetProp3D(coneActor);
  boxWidget->SetPlaceFactor(1.25); // Make the box 1.25x larger than the actor
  boxWidget->PlaceWidget();

  vtkNew<vtkMyCallback> callback;
  boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);

  boxWidget->On();

  window->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
