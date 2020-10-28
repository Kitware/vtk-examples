#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkCamera.h>
#include <vtkColor.h>
#include <vtkCubeAxesActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSuperquadricSource.h>
#include <vtkTextProperty.h>

int main(int, char*[])
{
  // Define colors for this example
  vtkNew<vtkNamedColors> colors;

  vtkColor3d backgroundColor = colors->GetColor3d("DarkSlateGray");
  vtkColor3d actorColor = colors->GetColor3d("Tomato");
  vtkColor3d axis1Color = colors->GetColor3d("Salmon");
  vtkColor3d axis2Color = colors->GetColor3d("PaleGreen");
  vtkColor3d axis3Color = colors->GetColor3d("LightSkyBlue");

  // Create a superquadric
  vtkNew<vtkSuperquadricSource> superquadricSource;
  superquadricSource->SetPhiRoundness(3.1);
  superquadricSource->SetThetaRoundness(1.0);
  superquadricSource->Update(); // needed to GetBounds later

  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(superquadricSource->GetOutputPort());

  vtkNew<vtkActor> superquadricActor;
  superquadricActor->SetMapper(mapper);
  superquadricActor->GetProperty()->SetDiffuseColor(actorColor.GetData());
  superquadricActor->GetProperty()->SetDiffuse(.7);
  superquadricActor->GetProperty()->SetSpecular(.7);
  superquadricActor->GetProperty()->SetSpecularPower(50.0);

  vtkNew<vtkCubeAxesActor> cubeAxesActor;
  cubeAxesActor->SetUseTextActor3D(1);
  cubeAxesActor->SetBounds(superquadricSource->GetOutput()->GetBounds());
  cubeAxesActor->SetCamera(renderer->GetActiveCamera());
  cubeAxesActor->GetTitleTextProperty(0)->SetColor(axis1Color.GetData());
  cubeAxesActor->GetTitleTextProperty(0)->SetFontSize(48);
  cubeAxesActor->GetLabelTextProperty(0)->SetColor(axis1Color.GetData());

  cubeAxesActor->GetTitleTextProperty(1)->SetColor(axis2Color.GetData());
  cubeAxesActor->GetLabelTextProperty(1)->SetColor(axis2Color.GetData());

  cubeAxesActor->GetTitleTextProperty(2)->SetColor(axis3Color.GetData());
  cubeAxesActor->GetLabelTextProperty(2)->SetColor(axis3Color.GetData());

  cubeAxesActor->DrawXGridlinesOn();
  cubeAxesActor->DrawYGridlinesOn();
  cubeAxesActor->DrawZGridlinesOn();
#if VTK_MAJOR_VERSION == 6
  cubeAxesActor->SetGridLineLocation(VTK_GRID_LINES_FURTHEST);
#endif
#if VTK_MAJOR_VERSION > 6
  cubeAxesActor->SetGridLineLocation(cubeAxesActor->VTK_GRID_LINES_FURTHEST);
#endif

  cubeAxesActor->XAxisMinorTickVisibilityOff();
  cubeAxesActor->YAxisMinorTickVisibilityOff();
  cubeAxesActor->ZAxisMinorTickVisibilityOff();

  cubeAxesActor->SetFlyModeToStaticEdges();
  renderer->AddActor(cubeAxesActor);
  renderer->AddActor(superquadricActor);
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);

  renderer->ResetCamera();
  renderer->SetBackground(backgroundColor.GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("CubeAxesActor");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renderer->GetActiveCamera()->Zoom(0.8);
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
