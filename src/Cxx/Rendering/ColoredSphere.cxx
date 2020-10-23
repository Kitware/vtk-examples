#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkElevationFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkSphereSource> sphere;
  sphere->SetPhiResolution(12);
  sphere->SetThetaResolution(12);

  vtkNew<vtkElevationFilter> colorIt;
  colorIt->SetInputConnection(sphere->GetOutputPort());
  colorIt->SetLowPoint(0, 0, -1);
  colorIt->SetHighPoint(0, 0, 1);

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(colorIt->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renWin->SetSize(640, 480);
  renWin->SetWindowName("ColoredSphere");

  renWin->Render();

  // interact with data
  iren->Start();

  return EXIT_SUCCESS;
}
