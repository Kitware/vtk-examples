#include <vtkCamera.h>
#include <vtkLight.h>
#include <vtkLightActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <cstdlib>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("Black").GetData());

  // Display where the light is
  vtkNew<vtkLight> light;
  light->SetPositional(true); // without this line, the program crashes
  vtkNew<vtkLightActor> lightActor;
  lightActor->SetLight(light);
  renderer->AddViewProp(lightActor);
  lightActor->GetFrustumProperty()->SetColor(
      colors->GetColor3d("LavenderBlush").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("LightActor");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->ResetCamera();
  renderer->ResetCameraClippingRange();

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(-2.17199, -2.50774, 2.18);
  camera->SetFocalPoint(-0.144661, -0.146372, 0.180482);
  camera->SetViewUp(0.0157883, 0.638203, 0.769706);
  camera->SetDistance(3.69921);
  camera->SetClippingRange(1.76133, 6.14753);

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
