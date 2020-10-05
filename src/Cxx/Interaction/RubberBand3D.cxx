#include <vtkActor.h>
#include <vtkInteractorStyleRubberBand3D.h>
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

class MyRubberBand3D : public vtkInteractorStyleRubberBand3D
{
public:
  static MyRubberBand3D* New();
  vtkTypeMacro(MyRubberBand3D, vtkInteractorStyleRubberBand3D);

  virtual void OnLeftButtonUp() override
  {
    // Forward events
    vtkInteractorStyleRubberBand3D::OnLeftButtonUp();

    std::cout << "Start position: " << this->StartPosition[0] << " "
              << this->StartPosition[1] << std::endl;
    std::cout << "End position: " << this->EndPosition[0] << " "
              << this->EndPosition[1] << std::endl;
  }
};
vtkStandardNewMacro(MyRubberBand3D);

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
  renderWindow->SetWindowName("RubberBand3D");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderWindow->Render();

  vtkNew<MyRubberBand3D> style;
  renderWindowInteractor->SetInteractorStyle(style);

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
