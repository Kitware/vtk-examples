#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <vector>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderWindow> renderWindow;

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  double xmins[4] = {0, .5, 0, .5};
  double xmaxs[4] = {0.5, 1, 0.5, 1};
  double ymins[4] = {0, 0, .5, .5};
  double ymaxs[4] = {0.5, 0.5, 1, 1};
  // Have some fun with colors
  std::vector<std::string> renBkg{"AliceBlue", "GhostWhite", "WhiteSmoke",
                                  "Seashell"};
  std::vector<std::string> actorColor{"Bisque", "RosyBrown", "Goldenrod",
                                      "Chocolate"};

  for (unsigned i = 0; i < 4; i++)
  {
    vtkNew<vtkRenderer> renderer;

    renderWindow->AddRenderer(renderer);
    renderer->SetViewport(xmins[i], ymins[i], xmaxs[i], ymaxs[i]);

    // Create a sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(0.0, 0.0, 0.0);
    sphereSource->SetRadius(5);
    sphereSource->Update();

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d(actorColor[i]).GetData());

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d(renBkg[i]).GetData());

    renderer->ResetCamera();
  }
  renderWindow->Render();
  renderWindow->SetWindowName("MultipleViewPorts");

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
