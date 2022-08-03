#include <vtkActor.h>
#include <vtkBorderRepresentation.h>
#include <vtkBorderWidget.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlatonicSolidSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>

namespace {
/** Get a specialised lookup table for the platonic solids.
 *
 * Since each face of a vtkPlatonicSolidSource has a different
 * cell scalar, we create a lookup table with a different colour
 * for each face.
 * The colors have been carefully chosen so that adjacent cells
 * are colored distinctly.
 *
 * @return The lookup table.
 */
vtkNew<vtkLookupTable> GetPlatonicLUT();

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

  auto lut = GetPlatonicLUT();

  vtkNew<vtkPlatonicSolidSource> source;
  source->SetSolidTypeToDodecahedron();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(source->GetOutputPort());
  mapper->SetLookupTable(lut);
  mapper->SetScalarRange(0, 19);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  // actor->GetProperty()->SetColor(
  //     colors->GetColor3d("DarkOliveGreen").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderer->GetActiveCamera()->Elevation(30.0);
  renderer->GetActiveCamera()->Azimuth(180.0);
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
  renderer->ResetCamera();
  renderWindow->Render();
  borderWidget->On();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

vtkNew<vtkLookupTable> GetPlatonicLUT()
{
  vtkNew<vtkLookupTable> lut;
  lut->SetNumberOfTableValues(20);
  lut->SetTableRange(0.0, 19.0);
  lut->Build();
  lut->SetTableValue(0, 0.1, 0.1, 0.1);
  lut->SetTableValue(1, 0, 0, 1);
  lut->SetTableValue(2, 0, 1, 0);
  lut->SetTableValue(3, 0, 1, 1);
  lut->SetTableValue(4, 1, 0, 0);
  lut->SetTableValue(5, 1, 0, 1);
  lut->SetTableValue(6, 1, 1, 0);
  lut->SetTableValue(7, 0.9, 0.7, 0.9);
  lut->SetTableValue(8, 0.5, 0.5, 0.5);
  lut->SetTableValue(9, 0.0, 0.0, 0.7);
  lut->SetTableValue(10, 0.5, 0.7, 0.5);
  lut->SetTableValue(11, 0, 0.7, 0.7);
  lut->SetTableValue(12, 0.7, 0, 0);
  lut->SetTableValue(13, 0.7, 0, 0.7);
  lut->SetTableValue(14, 0.7, 0.7, 0);
  lut->SetTableValue(15, 0, 0, 0.4);
  lut->SetTableValue(16, 0, 0.4, 0);
  lut->SetTableValue(17, 0, 0.4, 0.4);
  lut->SetTableValue(18, 0.4, 0, 0);
  lut->SetTableValue(19, 0.4, 0, 0.4);
  return lut;
}

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
  std::cout << "Upper right: " << lowerLeft[0] + upperRight[0] << " "
            << lowerLeft[1] + upperRight[1] << std::endl;

  vtkBorderWidget::EndSelectAction(w);
}

} // namespace
