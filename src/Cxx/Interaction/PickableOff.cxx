#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkConeSource> coneSource0;
  coneSource0->Update();

  vtkNew<vtkConeSource> coneSource1;
  coneSource1->SetCenter(2, 0, 0);
  coneSource1->Update();

  // Create mappers and actors
  vtkNew<vtkPolyDataMapper> coneMapper0;
  coneMapper0->SetInputConnection(coneSource0->GetOutputPort());

  vtkNew<vtkPolyDataMapper> coneMapper1;
  coneMapper1->SetInputConnection(coneSource1->GetOutputPort());

  // Create actors
  vtkNew<vtkActor> coneActor0;
  coneActor0->SetMapper(coneMapper0);
  coneActor0->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  vtkNew<vtkActor> coneActor1;
  coneActor1->SetMapper(coneMapper1);
  coneActor1->PickableOff();
  coneActor1->GetProperty()->SetColor(
      colors->GetColor3d("LightGoldenrodYellow").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("PickableOff");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(coneActor0);
  renderer->AddActor(coneActor1);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(0.9);
  renderWindow->Render();

  vtkNew<vtkInteractorStyleTrackballActor> style;

  renderWindowInteractor->SetInteractorStyle(style);

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
