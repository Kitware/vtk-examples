#include <vtkCamera.h>
#include <vtkCameraActor.h>
#include <vtkMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlanes.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> namedColors;

  // Sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(400);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetDiffuseColor(
      namedColors->GetColor3d("Tomato").GetData());

  // Camera
  vtkNew<vtkCamera> camera;

  vtkNew<vtkCameraActor> cameraActor;
  cameraActor->SetCamera(camera);
  cameraActor->GetProperty()->SetColor(
      namedColors->GetColor3d("Black").GetData());

  // (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
  auto bounds = cameraActor->GetBounds();
  std::cout << "bounds: " << bounds[0] << " " << bounds[1] << " " << bounds[2]
            << " " << bounds[3] << " " << bounds[4] << " " << bounds[5]
            << std::endl;

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("CameraActor");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(sphereActor);
  // Compute the active camera parameters
  renderer->ResetCamera();

  // Set the camera parameters for the camera actor
  camera->DeepCopy(renderer->GetActiveCamera());
  renderer->AddActor(cameraActor);

  // Position the camera so that we can see the camera actor
  renderer->GetActiveCamera()->SetPosition(1, 0, 0);
  renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);

  renderer->ResetCamera();
  renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
