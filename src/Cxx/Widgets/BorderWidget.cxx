#include <vtkActor.h>
#include <vtkBorderRepresentation.h>
#include <vtkBorderWidget.h>
#include <vtkCommand.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>

namespace {
class vtkCustomBorderWidget : public vtkBorderWidget
{
public:
  static vtkCustomBorderWidget* New();
  vtkTypeMacro(vtkCustomBorderWidget, vtkBorderWidget);

  static void EndSelectAction(vtkAbstractWidget* w);

  vtkCustomBorderWidget();
};

vtkStandardNewMacro(vtkCustomBorderWidget);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(4.0);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(
      colors->GetColor3d("DarkOliveGreen").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("BorderWidget");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkCustomBorderWidget> borderWidget;
  borderWidget->SetInteractor(renderWindowInteractor);
  borderWidget->CreateDefaultRepresentation();
  borderWidget->SelectableOff();

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  // Render an image (lights and cameras are created automatically)
  renderWindowInteractor->Initialize();
  renderWindow->Render();
  borderWidget->On();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkCustomBorderWidget::vtkCustomBorderWidget()
{
  this->CallbackMapper->SetCallbackMethod(
      vtkCommand::MiddleButtonReleaseEvent, vtkWidgetEvent::EndSelect, this,
      vtkCustomBorderWidget::EndSelectAction);
}

void vtkCustomBorderWidget::EndSelectAction(vtkAbstractWidget* w)
{
  vtkBorderWidget* borderWidget = dynamic_cast<vtkBorderWidget*>(w);

  // Get the actual box coordinates/planes
  // vtkNew<vtkPolyData> polydata;

  // Get the bottom left corner
  auto lowerLeft =
      static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
          ->GetPosition();
  std::cout << "Lower left: " << lowerLeft[0] << " " << lowerLeft[1]
            << std::endl;

  auto upperRight =
      static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
          ->GetPosition2();
  std::cout << "Upper right: " << upperRight[0] << " " << upperRight[1]
            << std::endl;

  vtkBorderWidget::EndSelectAction(w);
}
} // namespace