#include <vtkActor.h>
#include <vtkInteractorStyleTrackballActor.h>
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

namespace {

class MouseInteractorStyleShiftAndControl
  : public vtkInteractorStyleTrackballActor
{
public:
  static MouseInteractorStyleShiftAndControl* New();

  virtual void OnLeftButtonDown()
  {
    if (this->Interactor->GetShiftKey())
    {
      std::cout << "Shift held. ";
    }

    if (this->Interactor->GetControlKey())
    {
      std::cout << "Control held. ";
    }

    if (this->Interactor->GetAltKey())
    {
      std::cout << "Alt held. ";
    }

    std::cout << "Pressed left mouse button." << std::endl;

    // Forward events
    vtkInteractorStyleTrackballActor::OnLeftButtonDown();
  }
};

vtkStandardNewMacro(MouseInteractorStyleShiftAndControl);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;

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
  renderWindow->SetWindowName("ShiftAndControl");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render
  renderWindow->Render();

  vtkNew<MouseInteractorStyleShiftAndControl> style;

  renderWindowInteractor->SetInteractorStyle(style);

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
