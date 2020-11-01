#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;

  // Create a cone
  vtkNew<vtkConeSource> coneSource;
  coneSource->SetHeight(3);

  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(coneSource->GetOutputPort());
  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create a cube
  vtkNew<vtkCubeSource> cubeSource;

  vtkNew<vtkPolyDataMapper> cubeMapper;
  cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
  vtkNew<vtkActor> cubeActor;
  cubeActor->SetMapper(cubeMapper);
  cubeActor->GetProperty()->SetColor(colors->GetColor3d("Cornsilk").GetData());

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(
      colors->GetColor3d("Lavender").GetData());

  renderer->AddActor(sphereActor);

  vtkNew<vtkActorCollection> actorCollection;
  actorCollection->AddItem(cubeActor);
  actorCollection->AddItem(coneActor);
  actorCollection->InitTraversal();

  vtkNew<vtkTransform> transform;
  transform->PostMultiply(); // this is the key line
  transform->Translate(5.0, 0, 0);

  // actorCollection->SetUserTransform(transform);

  for (vtkIdType i = 0; i < actorCollection->GetNumberOfItems(); i++)
  {
    vtkActor* actor = actorCollection->GetNextActor();
    actor->SetUserTransform(transform);
    renderer->AddActor(actor);
  }

  // Create a renderer, render window, and interactor

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("TransformActorCollection");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
