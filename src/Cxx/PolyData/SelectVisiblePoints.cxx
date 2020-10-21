#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelectVisiblePoints.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace {
class MyInteractor : public vtkInteractorStyleTrackballCamera
{
public:
  static MyInteractor* New();
  vtkTypeMacro(MyInteractor, vtkInteractorStyleTrackballCamera);

  virtual void OnLeftButtonDown() override
  {
    this->VisibleFilter->Update();
    std::cout << "There are currently: "
              << this->VisibleFilter->GetOutput()->GetNumberOfPoints()
              << " visible." << std::endl;
    // Forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  }

  void SetVisibleFilter(vtkSmartPointer<vtkSelectVisiblePoints> vis)
  {
    this->VisibleFilter = vis;
  }

private:
  vtkSmartPointer<vtkSelectVisiblePoints> VisibleFilter;
};

vtkStandardNewMacro(MyInteractor);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(5.0, 0, 0);
  sphereSource->Update();

  vtkNew<vtkPointSource> pointSource;
  pointSource->SetRadius(2.0);
  pointSource->SetNumberOfPoints(200);
  pointSource->Update();

  // Create an actor and mapper
  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  vtkNew<vtkPolyDataMapper> pointsMapper;
  pointsMapper->SetInputConnection(pointSource->GetOutputPort());

  vtkNew<vtkActor> pointsActor;
  pointsActor->SetMapper(pointsMapper);
  sphereActor->GetProperty()->SetColor(
      colors->GetColor3d("Honeydew").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("SelectVisiblePoints");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(sphereActor);
  renderer->AddActor(pointsActor);
  renderer->SetBackground(colors->GetColor3d("ivory_black").GetData());

  vtkNew<vtkSelectVisiblePoints> selectVisiblePoints;
  selectVisiblePoints->SetInputConnection(pointSource->GetOutputPort());
  selectVisiblePoints->SetRenderer(renderer);
  selectVisiblePoints->Update();

  vtkNew<MyInteractor> style;
  renderWindowInteractor->SetInteractorStyle(style);
  style->SetVisibleFilter(selectVisiblePoints);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
