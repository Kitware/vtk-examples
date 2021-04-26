#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
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

#include <vector>

namespace {
std::vector<vtkSmartPointer<vtkPolyDataAlgorithm>> GetSources();
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderWindow> renderWindow;

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Define viewport ranges.
  double xmins[4] = {0, .5, 0, .5};
  double xmaxs[4] = {0.5, 1, 0.5, 1};
  double ymins[4] = {0, 0, .5, .5};
  double ymaxs[4] = {0.5, 0.5, 1, 1};
  // Have some fun with colors.
  std::vector<std::string> renBkg{"AliceBlue", "GhostWhite", "WhiteSmoke",
                                  "Seashell"};
  std::vector<std::string> actorColor{"Bisque", "RosyBrown", "Goldenrod",
                                      "Chocolate"};
  vtkCamera* camera = nullptr;
  auto sources = GetSources();

  for (unsigned i = 0; i < 4; i++)
  {
    vtkNew<vtkRenderer> renderer;

    renderWindow->AddRenderer(renderer);
    renderer->SetViewport(xmins[i], ymins[i], xmaxs[i], ymaxs[i]);

    // Share the camera between viewports.
    if (i == 0)
    {
      camera = renderer->GetActiveCamera();
      camera->Azimuth(30);
      camera->Elevation(30);
    }
    else
    {
      renderer->SetActiveCamera(camera);
    }

    // Create a mapper and actor.
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sources[i]->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d(actorColor[i]).GetData());

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d(renBkg[i]).GetData());

    renderer->ResetCamera();
  }
  renderWindow->Render();
  renderWindow->SetWindowName("MultipleViewPorts");
  renderWindow->SetSize(600, 600);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
std::vector<vtkSmartPointer<vtkPolyDataAlgorithm>> GetSources()
{
  std::vector<vtkSmartPointer<vtkPolyDataAlgorithm>> sources;
  for (unsigned i = 0; i < 4; i++)
  {
    // Create a sphere.
    vtkNew<vtkSphereSource> sphere;
    sphere->SetCenter(0.0, 0.0, 0.0);
    sphere->Update();
    sources.push_back(sphere);
    // Create a cone.
    vtkNew<vtkConeSource> cone;
    cone->SetCenter(0.0, 0.0, 0.0);
    cone->SetDirection(0, 1, 0);
    cone->Update();
    sources.push_back(cone);
    // Create a cube.
    vtkNew<vtkCubeSource> cube;
    cube->SetCenter(0.0, 0.0, 0.0);
    cube->Update();
    sources.push_back(cube);
    // Create a cylinder.
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetCenter(0.0, 0.0, 0.0);
    cylinder->Update();
    sources.push_back(cylinder);
  }
  return sources;
}

} // namespace
