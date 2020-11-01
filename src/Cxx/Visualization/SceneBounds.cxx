#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
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
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // Create a cube
  vtkNew<vtkCubeSource> cubeSource;
  cubeSource->SetCenter(5.0, 0.0, 0.0);
  cubeSource->Update();

  vtkNew<vtkPolyDataMapper> cubeMapper;
  cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
  vtkNew<vtkActor> cubeActor;
  cubeActor->SetMapper(cubeMapper);
  cubeActor->GetProperty()->SetColor(colors->GetColor3d("Thistle").GetData());

  // Visualization
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("SceneBounds");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(sphereActor);
  renderer->AddActor(cubeActor);
  renderer->SetBackground(colors->GetColor3d("CadetBlue").GetData());

  renderWindow->Render();
  renderer->GetActiveCamera()->Zoom(0.9);

  double bounds[6];
  renderer->ComputeVisiblePropBounds(bounds);
  std::cout << bounds[0] << " " << bounds[1] << " " << bounds[2] << " "
            << bounds[3] << " " << bounds[4] << " " << bounds[5] << std::endl;
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
