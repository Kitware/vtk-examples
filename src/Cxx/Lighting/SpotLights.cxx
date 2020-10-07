#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkLight.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  // create a rendering window and renderer
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 480);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("SpotLights");

  // create a renderwindowinteractor
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renderWindow);

  // create a plane and add it to the renderer
  vtkNew<vtkPlaneSource> source;
  source->SetOrigin(-5.0, 0.0, 5.0);
  source->SetPoint1(5.0, 0.0, 5.0);
  source->SetPoint2(-5.0, 0.0, -5.0);
  source->SetResolution(100, 100);
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  renderer->AddActor(actor);

  // set camera view
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(40.0);
  renderer->GetActiveCamera()->Elevation(30.0);

  // create a green light
  vtkNew<vtkLight> greenLight;
  greenLight->PositionalOn();
  greenLight->SetPosition(-4.0, 4.0, -1.0);
  greenLight->SetColor(colors->GetColor3d("Lawn_green").GetData());
  greenLight->SetIntensity(0.5);

  // create a pink light
  vtkNew<vtkLight> pinkLight;
  pinkLight->PositionalOn();
  pinkLight->SetPosition(4.0, 5.0, 1.0);
  pinkLight->SetColor(colors->GetColor3d("DeepPink").GetData());
  pinkLight->SetIntensity(0.6);

  // add lights to the renderer
  renderer->AddLight(pinkLight);
  renderer->AddLight(greenLight);

  // define a background for better visibility
  renderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());
  renderer->SetBackground2(colors->GetColor3d("SkyBlue").GetData());
  renderer->GradientBackgroundOn();

  // enable interactor
  iren->Initialize();
  renderWindow->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
