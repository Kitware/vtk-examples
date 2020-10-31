#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCullerCollection.h>
#include <vtkDataSetMapper.h>
#include <vtkElevationFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShrinkFilter.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  renderer->GetCullers()->RemoveAllItems();

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkSphereSource> sphere;
  sphere->SetThetaResolution(12);
  sphere->SetPhiResolution(12);

  vtkNew<vtkShrinkFilter> shrink;
  shrink->SetInputConnection(sphere->GetOutputPort());
  shrink->SetShrinkFactor(0.9);

  vtkNew<vtkElevationFilter> colorIt;
  colorIt->SetInputConnection(shrink->GetOutputPort());
  colorIt->SetLowPoint(0, 0, -.5);
  colorIt->SetHighPoint(0, 0, .5);

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(colorIt->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("LavenderBlush").GetData());
  renWin->SetSize(600, 600);
  renWin->SetWindowName("LoopShrink");

  renWin->Render();

  renderer->GetActiveCamera()->Zoom(1.5);

  // interact with data
  iren->Start();

  return EXIT_SUCCESS;
}
