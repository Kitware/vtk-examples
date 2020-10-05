#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSphereSource.h>

namespace {

// Handle mouse events
class MouseInteractorStyle2 : public vtkInteractorStyleTrackballCamera
{
public:
  static MouseInteractorStyle2* New();
  vtkTypeMacro(MouseInteractorStyle2, vtkInteractorStyleTrackballCamera);
  vtkNew<vtkNamedColors> colors;

  virtual void OnLeftButtonDown() override
  {
    int* clickPos = this->GetInteractor()->GetEventPosition();

    // Pick from this location.
    vtkNew<vtkPropPicker> picker;
    picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

    double* pos = picker->GetPickPosition();
    std::cout << "Pick position (world coordinates) is: " << pos[0] << " "
              << pos[1] << " " << pos[2] << std::endl;

    auto pickedActor = picker->GetActor();
    if (pickedActor == nullptr)
    {
      std::cout << "No actor picked." << std::endl;
    }
    else
    {
      std::cout << "Picked actor: " << picker->GetActor() << std::endl;
      // Create a sphere
      // Create a sphere
      vtkNew<vtkSphereSource> sphereSource;
      sphereSource->SetCenter(pos[0], pos[1], pos[2]);
      sphereSource->SetRadius(0.1);

      // Create a mapper and actor
      vtkNew<vtkPolyDataMapper> mapper;
      mapper->SetInputConnection(sphereSource->GetOutputPort());

      vtkNew<vtkActor> actor;
      actor->SetMapper(mapper);
      actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

      this->GetDefaultRenderer()->AddActor(actor);
      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
  }

private:
};

vtkStandardNewMacro(MouseInteractorStyle2);

} // namespace

// Execute application.
int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPlaneSource> planeSource;
  planeSource->Update();

  // Create a polydata object
  vtkPolyData* polydata = planeSource->GetOutput();

  // Create a mapper
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polydata);

  // Create an actor
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(
      colors->GetColor3d("LightGoldenrodYellow").GetData());

  std::cout << "Actor address: " << actor << std::endl;

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Picking");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Set the custom stype to use for interaction.
  vtkNew<MouseInteractorStyle2> style;
  style->SetDefaultRenderer(renderer);

  renderWindowInteractor->SetInteractorStyle(style);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DodgerBlue").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
