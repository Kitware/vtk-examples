#include <vtkActor.h>
#include <vtkBiDimensionalRepresentation2D.h>
#include <vtkBiDimensionalWidget.h>
#include <vtkCommand.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {
class vtkBiDimensionalCallback : public vtkCommand
{
public:
  static vtkBiDimensionalCallback* New()
  {
    return new vtkBiDimensionalCallback;
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {

    vtkBiDimensionalWidget* biDimensionalWidget =
        reinterpret_cast<vtkBiDimensionalWidget*>(caller);

    vtkBiDimensionalRepresentation2D* representation =
        static_cast<vtkBiDimensionalRepresentation2D*>(
            biDimensionalWidget->GetRepresentation());

    double p1[3];
    representation->GetPoint1DisplayPosition(p1);
    double p2[3];
    representation->GetPoint1DisplayPosition(p2);
    double p3[3];
    representation->GetPoint1DisplayPosition(p3);
    double p4[3];
    representation->GetPoint1DisplayPosition(p4);

    // Display one of the points, just so we know it's working
    std::cout << "P1: " << p1[0] << " " << p1[1] << " " << p1[2] << std::endl;
  }
  vtkBiDimensionalCallback()
  {
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
  actor->GetProperty()->SetColor(
      colors->GetColor3d("DarkOliveGreen").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("BiDimensionalWidget");

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkBiDimensionalWidget> biDimensionalWidget;
  biDimensionalWidget->SetInteractor(renderWindowInteractor);
  biDimensionalWidget->CreateDefaultRepresentation();

  vtkNew<vtkBiDimensionalCallback> biDimensionalCallback;

  biDimensionalWidget->AddObserver(vtkCommand::InteractionEvent,
                                   biDimensionalCallback);

  // Render an image (lights and cameras are created automatically)
  renderWindow->Render();

  renderWindowInteractor->Initialize();
  renderWindow->Render();
  biDimensionalWidget->On();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
