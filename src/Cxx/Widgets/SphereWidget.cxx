#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereRepresentation.h>
#include <vtkSphereSource.h>
#include <vtkSphereWidget.h>

namespace {
class SphereCallback : public vtkCommand
{
public:
  static SphereCallback* New()
  {
    return new SphereCallback;
  }
  SphereCallback()
  {
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {

    vtkSphereWidget* sphereWidget = reinterpret_cast<vtkSphereWidget*>(caller);

    double center[3];
    sphereWidget->GetCenter(center);
    std::cout << "Center: " << center[0] << " " << center[1] << " " << center[2]
              << std::endl;
  }
};
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("SphereWidget");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkSphereWidget> sphereWidget;
  sphereWidget->SetInteractor(renderWindowInteractor);
  sphereWidget->SetRepresentationToSurface();
  sphereWidget->GetSphereProperty()->SetColor(
      colors->GetColor3d("BurlyWood").GetData());

  vtkNew<SphereCallback> sphereCallback;

  sphereWidget->AddObserver(vtkCommand::InteractionEvent, sphereCallback);

  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindow->Render();
  sphereWidget->On();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
