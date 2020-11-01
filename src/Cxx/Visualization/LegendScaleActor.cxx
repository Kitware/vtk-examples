#include <vtkActor.h>
#include <vtkLegendScaleActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricEnneper.h>
#include <vtkParametricFunctionSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // The next source will be a parametric function
  vtkNew<vtkParametricEnneper> src;
  vtkNew<vtkParametricFunctionSource> fnSrc;
  fnSrc->SetParametricFunction(src);

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(fnSrc->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("LegendScaleActor");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkLegendScaleActor> legendScaleActor;

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->AddActor(legendScaleActor);

  renderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
