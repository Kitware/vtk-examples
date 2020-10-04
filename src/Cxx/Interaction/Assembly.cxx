#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkAxesActor.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> namedColors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(
      namedColors->GetColor3d("Banana").GetData());

  // Create a cube
  vtkNew<vtkCubeSource> cubeSource;
  cubeSource->SetCenter(5.0, 0.0, 0.0);
  cubeSource->Update();

  vtkNew<vtkPolyDataMapper> cubeMapper;
  cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
  vtkNew<vtkActor> cubeActor;
  cubeActor->SetMapper(cubeMapper);
  cubeActor->GetProperty()->SetColor(
      namedColors->GetColor3d("Tomato").GetData());
  // Combine the sphere and cube into an assembly
  vtkNew<vtkAssembly> assembly;
  assembly->AddPart(sphereActor);
  assembly->AddPart(cubeActor);

  // Apply a transform to the whole assembly
  vtkNew<vtkTransform> transform;
  transform->PostMultiply(); // this is the key line
  transform->Translate(5.0, 0, 0);

  assembly->SetUserTransform(transform);

  // Extract each actor from the assembly and change its opacity
  vtkNew<vtkPropCollection> collection;

  assembly->GetActors(collection);
  collection->InitTraversal();
  for (vtkIdType i = 0; i < collection->GetNumberOfItems(); i++)
  {
    dynamic_cast<vtkActor*>(collection->GetNextProp())
        ->GetProperty()
        ->SetOpacity(0.5);
  }

  // Visualization
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(assembly);
  renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

  renderer->ResetCamera();
  renderWindow->SetWindowName("Assembly");
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
