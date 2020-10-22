#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkColorSeries.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <map>
#include <vector>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // We store background colors in a vector. Then we extract the red, green and
  // blue components later when coloring the reneder background.
  std::vector<vtkColor3ub> rendererColors;
  vtkNew<vtkColorSeries> colorSeries;
  colorSeries->SetColorSchemeByName("Brewer Qualitative Pastel2");
  rendererColors.push_back(colorSeries->GetColor(0));
  rendererColors.push_back(colorSeries->GetColor(1));
  rendererColors.push_back(colorSeries->GetColor(2));
  rendererColors.push_back(colorSeries->GetColor(3));

  vtkNew<vtkRenderWindow> renderWindow;

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  double xmins[4] = {0, 0.5, 0, 0.5};
  double xmaxs[4] = {0.5, 1, 0.5, 1};
  double ymins[4] = {0, 0, 0.5, 0.5};
  double ymaxs[4] = {0.5, 0.5, 1, 1};

  vtkCamera* camera = nullptr;

  // Using the superclass for the sources
  std::vector<vtkSmartPointer<vtkPolyDataAlgorithm>> sources;

  for (unsigned i = 0; i < 4; i++)
  {
    switch (i)
    {
    case 0: {
      // Create a sphere
      vtkNew<vtkSphereSource> sphereSource;
      sphereSource->SetCenter(0.0, 0.0, 0.0);
      sphereSource->Update();
      sources.push_back(sphereSource);
      break;
    }
    case 1: {
      // Create a cone
      vtkNew<vtkConeSource> coneSource;
      coneSource->SetCenter(0.0, 0.0, 0.0);
      coneSource->Update();
      sources.push_back(coneSource);
      break;
    }
    case 2: {
      // Create a cube
      vtkNew<vtkCubeSource> cubeSource;
      cubeSource->SetCenter(0.0, 0.0, 0.0);
      cubeSource->Update();
      sources.push_back(cubeSource);
      break;
    }
    default:
    case 3: {
      // Create a cylinder
      vtkNew<vtkCylinderSource> cylinderSource;
      cylinderSource->SetCenter(0.0, 0.0, 0.0);
      cylinderSource->Update();
      sources.push_back(cylinderSource);
    }
    };

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sources[i]->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    double r, g, b;
    r = rendererColors[i].GetRed() / 255.0;
    g = rendererColors[i].GetGreen() / 255.0;
    b = rendererColors[i].GetBlue() / 255.0;
    renderer->SetBackground(r, g, b);

    renderWindow->AddRenderer(renderer);

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
    renderer->SetViewport(xmins[i], ymins[i], xmaxs[i], ymaxs[i]);

    renderer->ResetCamera();
  }

  renderWindow->Render();
  renderWindow->SetWindowName("ShareCamera");

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
