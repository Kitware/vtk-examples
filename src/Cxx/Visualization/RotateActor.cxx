#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkMatrix4x4.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {

// Define interaction style
class MyInteractorStyle : public vtkInteractorStyleTrackballActor
{
public:
  static MyInteractorStyle* New();
  vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleTrackballActor);

  virtual void OnLeftButtonDown()
  {
    std::cout << "Pressed left mouse button." << std::endl;

    vtkNew<vtkMatrix4x4> m;
    this->Actor->GetMatrix(m);
    std::cout << "Matrix: " << endl << *m << std::endl;

    // Forward events
    vtkInteractorStyleTrackballActor::OnLeftButtonDown();
  }

  virtual void OnLeftButtonUp()
  {
    std::cout << "Released left mouse button." << std::endl;

    vtkNew<vtkMatrix4x4> m;
    this->Actor->GetMatrix(m);
    std::cout << "Matrix: " << endl << *m << std::endl;

    // Forward events
    vtkInteractorStyleTrackballActor::OnLeftButtonUp();
  }

  void SetActor(vtkSmartPointer<vtkActor> actor)
  {
    this->Actor = actor;
  }

private:
  vtkSmartPointer<vtkActor> Actor;
};
vtkStandardNewMacro(MyInteractorStyle);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a cone
  vtkNew<vtkConeSource> coneSource;
  coneSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(coneSource->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->RotateY(45);
  actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("RotateActor");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<MyInteractorStyle> style;
  style->SetActor(actor);

  renderWindowInteractor->SetInteractorStyle(style);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(
      colors->GetColor3d("DarkSlateGray").GetData()); // white

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
