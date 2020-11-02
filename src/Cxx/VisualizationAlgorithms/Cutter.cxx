#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkCutter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkCubeSource> cube;
  cube->SetXLength(40);
  cube->SetYLength(30);
  cube->SetZLength(20);
  vtkNew<vtkPolyDataMapper> cubeMapper;
  cubeMapper->SetInputConnection(cube->GetOutputPort());

  // Create a plane to cut,here it cuts in the XZ direction (xz
  // normal=(1,0,0);XY =(0,0,1),YZ =(0,1,0)
  vtkNew<vtkPlane> plane;
  plane->SetOrigin(10, 0, 0);
  plane->SetNormal(1, 0, 0);

  // Create cutter
  vtkNew<vtkCutter> cutter;
  cutter->SetCutFunction(plane);
  cutter->SetInputConnection(cube->GetOutputPort());
  cutter->Update();

  vtkNew<vtkPolyDataMapper> cutterMapper;
  cutterMapper->SetInputConnection(cutter->GetOutputPort());
  cutterMapper->SetResolveCoincidentTopologyToPolygonOffset();

  // Create plane actor
  vtkNew<vtkActor> planeActor;
  planeActor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());
  planeActor->GetProperty()->SetLineWidth(2);
  planeActor->GetProperty()->SetAmbient(1.0);
  planeActor->GetProperty()->SetDiffuse(0.0);
  planeActor->SetMapper(cutterMapper);

  // Create cube actor
  vtkNew<vtkActor> cubeActor;
  cubeActor->GetProperty()->SetColor(
      colors->GetColor3d("Aquamarine").GetData());
  cubeActor->GetProperty()->SetOpacity(0.5);
  cubeActor->SetMapper(cubeMapper);

  // Create renderers and add actors of plane and cube
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(planeActor); // display the rectangle resulting from the
                                  // cut
  renderer->AddActor(cubeActor);  // display the cube

  // Add renderer to renderwindow and render
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(600, 600);
  renderWindow->SetWindowName("Cutter");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());
  renderWindow->Render();

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(-37.2611, -86.2155, 44.841);
  camera->SetFocalPoint(0.569422, -1.65124, -2.49482);
  camera->SetViewUp(0.160129, 0.42663, 0.890138);
  camera->SetDistance(104.033);
  camera->SetClippingRange(55.2019, 165.753);

  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
}
