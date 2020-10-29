#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkFollower.h>
#include <vtkInteractorStyle.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVectorText.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create some text
  vtkNew<vtkVectorText> textSource;
  textSource->SetText("Hello");

  // Create a mapper
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(textSource->GetOutputPort());

  // Create a subclass of vtkActor: a vtkFollower that remains facing the camera
  vtkNew<vtkFollower> follower;
  follower->SetMapper(mapper);
  follower->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  // Create a transparent cube that does not move around to face the camera
  vtkNew<vtkCubeSource> cubeSource;
  vtkNew<vtkPolyDataMapper> cubeMapper;
  cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
  vtkNew<vtkActor> cubeActor;
  cubeActor->SetMapper(cubeMapper);
  cubeActor->GetProperty()->SetColor(
      colors->GetColor3d("MidnightBlue").GetData());
  cubeActor->GetProperty()->SetOpacity(0.3);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Follower");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  dynamic_cast<vtkInteractorStyle*>(
      renderWindowInteractor->GetInteractorStyle())
      ->AutoAdjustCameraClippingRangeOn();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(follower);
  renderer->AddActor(cubeActor);
  renderer->SetBackground(colors->GetColor3d("LightSlateGray").GetData());
  follower->SetCamera(renderer->GetActiveCamera());

  // Render and interact
  renderWindow->Render();
  renderer->ResetCamera();
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
