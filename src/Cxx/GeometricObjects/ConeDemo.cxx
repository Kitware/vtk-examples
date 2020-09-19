#include <vtkNew.h>
#include <vtkConeSource.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkColor.h>

int main(int, char *[])
{
  vtkNew<vtkNamedColors> colors;

  // Define all of the colors used in the example
  vtkColor3d backgroundColor = colors->GetColor3d("tan");
  vtkColor3d actorColor = colors->GetColor3d("orchid");
  vtkColor3d actorBackfaceColor = colors->GetColor3d("PowderBlue");
  vtkColor3d coneLineColor = colors->GetColor3d("Black");

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("ConeDemo");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Shared camera
  vtkNew<vtkCamera> camera;

  // Define viewport ranges
  double xmins[4] = {0, .5, 0, .5};
  double xmaxs[4] = {0.5, 1, 0.5, 1};
  double ymins[4] = {0, 0, .5, .5};
  double ymaxs[4] = {0.5, 0.5, 1, 1};

  // Each viewport will contain a cone with an increasing resolution
  for (unsigned i = 0; i < 4; i++)
  {
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(backgroundColor.GetData());

    renderWindow->AddRenderer(renderer);
    renderer->SetViewport(xmins[i], ymins[i], xmaxs[i], ymaxs[i]);

    // Create a cone with different resolutions
    vtkNew<vtkConeSource> coneSource;
    coneSource->SetResolution(i);
    coneSource->SetDirection(0, 1, 0);
    if (i == 3)
    {
      coneSource->SetResolution(20);
    }

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(coneSource->GetOutputPort());

    vtkNew<vtkProperty> backfaceProp;
    backfaceProp->SetDiffuseColor(actorBackfaceColor.GetData());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    if (i > 0)
    {
      actor->GetProperty()->SetDiffuseColor(actorColor.GetData());
    }
    else
    {
      actor->GetProperty()->SetDiffuseColor(coneLineColor.GetData());
    }
    actor->GetProperty()->SetLineWidth(2);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->SetBackfaceProperty(backfaceProp);
    renderer->AddActor(actor);
    renderer->SetActiveCamera(camera);
    if (i == 3)
    {
      renderer->ResetCamera();
    }
  }
  camera->Azimuth(120);
  camera->Elevation(45);
  camera->Dolly(1.1);

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style);
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
