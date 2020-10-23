#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <array>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Set the background color.
  std::array<unsigned char, 4> cubeColor{{250, 128, 114, 255}};
  colors->SetColor("CubeColor", cubeColor.data());
  std::array<unsigned char, 4> bkg{{230, 230, 230, 255}};
  colors->SetColor("BkgColor", bkg.data());

  // create rendering windows and three renderers
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderer> ren2;
  vtkNew<vtkRenderWindow> renWindow1;
  renWindow1->AddRenderer(ren1);
  renWindow1->AddRenderer(ren2);
  renWindow1->SetWindowName("Model");

  vtkNew<vtkRenderWindowInteractor> iren1;
  iren1->SetRenderWindow(renWindow1);
  vtkNew<vtkRenderer> ren3;
  vtkNew<vtkRenderWindow> renWindow2;
  renWindow2->AddRenderer(ren3);
  renWindow2->SetWindowName("Model");

  vtkNew<vtkRenderWindowInteractor> iren2;
  iren2->SetRenderWindow(renWindow2);

  // create an actor and give it cone geometry
  vtkNew<vtkConeSource> cone;
  cone->SetResolution(8);

  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(cone->GetOutputPort());

  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  // create an actor and give it cube geometry
  vtkNew<vtkCubeSource> cube;

  vtkNew<vtkPolyDataMapper> cubeMapper;
  cubeMapper->SetInputConnection(cube->GetOutputPort());

  vtkNew<vtkActor> cubeActor;
  cubeActor->SetMapper(cubeMapper);
  cubeActor->GetProperty()->SetColor(colors->GetColor3d("CubeColor").GetData());

  // create an actor and give it sphere geometry
  vtkNew<vtkSphereSource> sphere;
  sphere->SetThetaResolution(16);
  sphere->SetPhiResolution(16);

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphere->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(colors->GetColor3d("Melon").GetData());

  // assign our actor to both renderers
  ren1->AddActor(coneActor);
  ren2->AddActor(sphereActor);
  ren3->AddActor(cubeActor);

  // set the size of our window
  renWindow1->SetSize(300, 150);
  renWindow1->SetPosition(0, 50);
  renWindow2->SetSize(300, 300);
  renWindow2->SetPosition(0, 300);

  // set the viewports and background of the renderers
  ren1->SetViewport(0, 0, 0.5, 1);
  ren1->SetBackground(colors->GetColor3d("BkgColor").GetData());
  ren2->SetViewport(0.5, 0, 1, 1);
  ren2->SetBackground(colors->GetColor3d("Linen").GetData());
  ren3->SetBackground(colors->GetColor3d("Honeydew").GetData());

  // draw the resulting scene
  renWindow1->Render();
  renWindow2->Render();

  iren1->Start();

  return EXIT_SUCCESS;
}
