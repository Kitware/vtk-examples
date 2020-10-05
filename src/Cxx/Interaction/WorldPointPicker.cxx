#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSphereSource.h>
#include <vtkWorldPointPicker.h>

namespace {

// Define interaction style
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
  static MouseInteractorStyle* New();
  vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);

  virtual void OnLeftButtonDown() override
  {
    std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0]
              << " " << this->Interactor->GetEventPosition()[1] << std::endl;
    this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
                                        this->Interactor->GetEventPosition()[1],
                                        0, // always zero.
                                        this->Interactor->GetRenderWindow()
                                            ->GetRenderers()
                                            ->GetFirstRenderer());
    double picked[3];
    this->Interactor->GetPicker()->GetPickPosition(picked);
    std::cout << "Picked value: " << picked[0] << " " << picked[1] << " "
              << picked[2] << std::endl;
    // Forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  }
};
vtkStandardNewMacro(MouseInteractorStyle);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkWorldPointPicker> worldPointPicker;

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("WorldPointPicker");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetPicker(worldPointPicker);
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<MouseInteractorStyle> style;
  renderWindowInteractor->SetInteractorStyle(style);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
