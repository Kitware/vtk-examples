#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a cone
  vtkNew<vtkConeSource> coneSource1;
  coneSource1->Update();

  vtkNew<vtkConeSource> coneSource2;
  coneSource2->Update();

  vtkNew<vtkPolyDataMapper> mapper1;
  mapper1->SetInputConnection(coneSource1->GetOutputPort());
  vtkNew<vtkActor> actor1;
  actor1->SetMapper(mapper1);
  actor1->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create a second, transformed cone
  vtkNew<vtkPolyDataMapper> mapper2;
  mapper2->SetInputConnection(coneSource2->GetOutputPort());
  vtkNew<vtkActor> actor2;
  actor2->SetMapper(mapper2);
  actor2->GetProperty()->SetColor(colors->GetColor3d("Cornsilk").GetData());

  vtkNew<vtkTransform> transform;
  transform->PostMultiply(); // this is the key line
  transform->RotateZ(90.0);

  actor2->SetUserTransform(transform);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("TransformActor");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor1);
  renderer->AddActor(actor2);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
