#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkReflectionFilter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkConeSource> coneSource;
  coneSource->Update();

  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(coneSource->GetOutputPort());
  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(colors->GetColor3d("Mint").GetData());

  // Reflection
  vtkNew<vtkReflectionFilter> reflectionFilter;
  reflectionFilter->SetInputConnection(coneSource->GetOutputPort());
  reflectionFilter->CopyInputOff();
  reflectionFilter->Update();

  vtkNew<vtkDataSetMapper> reflectionMapper;
  reflectionMapper->SetInputConnection(reflectionFilter->GetOutputPort());
  vtkNew<vtkActor> reflectionActor;
  reflectionActor->SetMapper(reflectionMapper);
  reflectionActor->GetProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 480);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Reflection");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(coneActor);
  renderer->AddActor(reflectionActor);
  renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();
  return EXIT_SUCCESS;
}
