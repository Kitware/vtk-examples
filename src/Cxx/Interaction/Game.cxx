// Demonstrate moving pieces and "snapping"

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkLinearTransform.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

namespace {

// Define interaction style
class MouseInteractorStyle6 : public vtkInteractorStyleTrackballActor
{
public:
  static MouseInteractorStyle6* New();
  vtkTypeMacro(MouseInteractorStyle6, vtkInteractorStyleTrackballActor);

  virtual void OnLeftButtonDown() override
  {
    std::cout << "Pressed left mouse button." << std::endl;
    // Forward events
    vtkInteractorStyleTrackballActor::OnLeftButtonDown();
  }

  virtual void OnMiddleButtonUp() override
  {
    // std::cout << "Pressed middle mouse button." << std::endl;

    int x = this->Interactor->GetEventPosition()[0];
    int y = this->Interactor->GetEventPosition()[1];
    this->FindPokedRenderer(x, y);
    this->FindPickedActor(x, y);

    if (this->CurrentRenderer == NULL || this->InteractionProp == NULL)
    {
      std::cout << "Nothing selected." << std::endl;
      return;
    }

    vtkNew<vtkPropCollection> actors;

    this->InteractionProp->GetActors(actors);
    actors->InitTraversal();
    vtkActor* actor = dynamic_cast<vtkActor*>(actors->GetNextProp());

    vtkPolyData* polydata =
        dynamic_cast<vtkPolyData*>(actor->GetMapper()->GetInputAsDataSet());

    vtkNew<vtkTransform> transform;
    transform->SetMatrix(actor->GetMatrix());

    vtkNew<vtkTransformPolyDataFilter> transformPolyData;
    transformPolyData->SetInputData(polydata);
    transformPolyData->SetTransform(transform);
    transformPolyData->Update();

    vtkNew<vtkSelectEnclosedPoints> selectEnclosedPoints;
    selectEnclosedPoints->SetInputConnection(
        transformPolyData->GetOutputPort());
    selectEnclosedPoints->SetSurfaceData(this->Sphere);
    selectEnclosedPoints->Update();

    vtkDataArray* insideArray = dynamic_cast<vtkDataArray*>(
        selectEnclosedPoints->GetOutput()->GetPointData()->GetArray(
            "SelectedPoints"));

    bool inside = false;
    for (vtkIdType i = 0; i < insideArray->GetNumberOfTuples(); i++)
    {
      if (insideArray->GetComponent(i, 0) == 1)
      {
        inside = true;
        break;
      }
    }

    if (inside)
    {
      std::cout << "A point of the cube is inside the sphere!" << std::endl;
      // Reset the cube to its original position
      // this->CubeActor->GetMatrix()->Identity();
      // this->CubeActor->SetOrigin(0,0,0);
      this->CubeActor->SetPosition(0, 0, 0);
      this->CubeActor->SetOrientation(0, 0, 0);

      this->Interactor->GetRenderWindow()
          ->GetRenderers()
          ->GetFirstRenderer()
          ->Render();
      this->Interactor->GetRenderWindow()->Render();
    }

    // Release interaction
    this->StopState();
  }

  virtual void OnRightButtonDown() override
  {
    std::cout << "Pressed right mouse button." << std::endl;
    // Forward events
    vtkInteractorStyleTrackballActor::OnRightButtonDown();
  }

  vtkPolyData* Sphere = nullptr;
  vtkActor* CubeActor = nullptr;
};
vtkStandardNewMacro(MouseInteractorStyle6);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> color;

  // Sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(2);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(color->GetColor3d("Tomato").GetData());

  // Cube
  vtkNew<vtkCubeSource> cubeSource;
  cubeSource->SetCenter(5.0, 0.0, 0.0);
  cubeSource->Update();

  vtkNew<vtkPolyDataMapper> cubeMapper;
  cubeMapper->SetInputConnection(cubeSource->GetOutputPort());

  vtkNew<vtkActor> cubeActor;
  cubeActor->SetMapper(cubeMapper);
  cubeActor->GetProperty()->SetColor(color->GetColor3d("DodgerBlue").GetData());

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Game");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(sphereActor);
  renderer->AddActor(cubeActor);
  renderer->SetBackground(color->GetColor3d("Burlywood").GetData());

  renderWindow->Render();

  vtkNew<MouseInteractorStyle6> style;
  style->Sphere = sphereSource->GetOutput();
  style->CubeActor = cubeActor;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
