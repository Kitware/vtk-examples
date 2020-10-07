#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkLight.h>
#include <vtkLightActor.h>
#include <vtkLightCollection.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <cstdlib>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("Black").GetData());

  double lightPosition[3] = {0, 0, 1};

  // Create a light
  double lightFocalPoint[3] = {0, 0, 0};

  vtkNew<vtkLight> light;
  light->SetLightTypeToSceneLight();
  light->SetPosition(lightPosition[0], lightPosition[1], lightPosition[2]);
  light->SetPositional(true); // required for vtkLightActor below
  light->SetConeAngle(10);
  light->SetFocalPoint(lightFocalPoint[0], lightFocalPoint[1],
                       lightFocalPoint[2]);
  light->SetDiffuseColor(colors->GetColor3d("Red").GetData());
  light->SetAmbientColor(colors->GetColor3d("Green").GetData());
  light->SetSpecularColor(colors->GetColor3d("Blue").GetData());

  vtkLightCollection* originalLights = renderer->GetLights();
  std::cout << "Originally there are " << originalLights->GetNumberOfItems()
            << " lights." << std::endl;

  //  renderer->AddLight(light); // can't do this here - must do this after the
  //  renderWindow->Render() below

  // Display where the light is
  vtkNew<vtkLightActor> lightActor;
  lightActor->SetLight(light);
  renderer->AddViewProp(lightActor);

  // Display where the light is focused
  vtkNew<vtkSphereSource> lightFocalPointSphere;
  lightFocalPointSphere->SetCenter(lightFocalPoint);
  lightFocalPointSphere->SetRadius(0.1);
  lightFocalPointSphere->Update();

  vtkNew<vtkPolyDataMapper> lightFocalPointMapper;
  lightFocalPointMapper->SetInputConnection(
      lightFocalPointSphere->GetOutputPort());

  vtkNew<vtkActor> lightFocalPointActor;
  lightFocalPointActor->SetMapper(lightFocalPointMapper);
  lightFocalPointActor->GetProperty()->SetColor(
      colors->GetColor3d("Yellow").GetData());
  renderer->AddViewProp(lightFocalPointActor);

  // Create a plane for the light to shine on
  vtkNew<vtkPlaneSource> planeSource;
  planeSource->SetResolution(100, 100);
  planeSource->Update();

  vtkNew<vtkPolyDataMapper> planeMapper;
  planeMapper->SetInputData(planeSource->GetOutput());

  vtkNew<vtkActor> planeActor;
  planeActor->SetMapper(planeMapper);
  renderer->AddActor(planeActor);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Light");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  std::cout << "Now there are " << originalLights->GetNumberOfItems()
            << " lights." << std::endl;

  renderer->AddLight(light); // must do this after renderWindow->Render();
  std::cout << "Now there are " << originalLights->GetNumberOfItems()
            << " lights." << std::endl;

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(-2.17199, -2.50774, 2.18);
  camera->SetFocalPoint(-0.144661, -0.146372, 0.180482);
  camera->SetViewUp(0.0157883, 0.638203, 0.769706);
  camera->SetDistance(3.69921);
  camera->SetClippingRange(1.76133, 6.14753);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
