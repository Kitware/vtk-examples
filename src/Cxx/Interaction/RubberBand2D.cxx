#include <vtkActor.h>
#include <vtkInteractorStyleRubberBand2D.h>
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

class MyRubberBand : public vtkInteractorStyleRubberBand2D
{
public:
  static MyRubberBand* New();
  vtkTypeMacro(MyRubberBand, vtkInteractorStyleRubberBand2D);

  virtual void OnLeftButtonUp() override
  {
    // Forward events
    vtkInteractorStyleRubberBand2D::OnLeftButtonUp();

    std::cout << "Start position: " << this->StartPosition[0] << " "
              << this->StartPosition[1] << std::endl;
    std::cout << "End position: " << this->EndPosition[0] << " "
              << this->EndPosition[1] << std::endl;
  }
};

vtkStandardNewMacro(MyRubberBand);

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
  renderWindow->SetWindowName("RubberBand2D");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render
  renderWindow->Render();

  vtkNew<MyRubberBand> style;
  renderWindowInteractor->SetInteractorStyle(style);

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
