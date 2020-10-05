#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {

// Handle mouse events
class MouseInteractorStyle5 : public vtkInteractorStyleTrackballActor
{
public:
  static MouseInteractorStyle5* New();
  vtkTypeMacro(MouseInteractorStyle5, vtkInteractorStyleTrackballActor);

  virtual void OnLeftButtonDown() override
  {
    // Forward events
    vtkInteractorStyleTrackballActor::OnLeftButtonDown();

    if (this->InteractionProp == this->Cube)
    {
      std::cout << "Picked cube." << std::endl;
    }
    else if (this->InteractionProp == this->Sphere)
    {
      std::cout << "Picked sphere." << std::endl;
    }
  }

  vtkActor* Cube;
  vtkActor* Sphere;
};

vtkStandardNewMacro(MouseInteractorStyle5);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a cube
  vtkNew<vtkCubeSource> cubeSource;
  cubeSource->Update();

  vtkNew<vtkPolyDataMapper> cubeMapper;
  cubeMapper->SetInputConnection(cubeSource->GetOutputPort());

  vtkNew<vtkActor> cubeActor;
  cubeActor->SetMapper(cubeMapper);
  cubeActor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(2, 0, 0);
  sphereSource->Update();

  // Create a mapper
  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

  // Create an actor
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(
      colors->GetColor3d("LightGoldenrodYellow").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("SelectAnActor");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Set the custom stype to use for interaction.
  vtkNew<MouseInteractorStyle5> style;
  style->SetDefaultRenderer(renderer);
  style->Cube = cubeActor;
  style->Sphere = sphereActor;

  renderWindowInteractor->SetInteractorStyle(style);

  renderer->AddActor(cubeActor);
  renderer->AddActor(sphereActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(0.9);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
