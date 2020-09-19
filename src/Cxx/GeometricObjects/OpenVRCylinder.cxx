#include <vtkActor.h>
#include <vtkCylinderSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderer.h>
#include <vtkOpenVRRenderWindowInteractor.h>

int main(int, char *argv[])
{
  // Create a sphere
  vtkNew<vtkCylinderSource> cylinderSource;
  cylinderSource->SetCenter(0.0, 0.0, 0.0);
  cylinderSource->SetRadius(5.0);
  cylinderSource->SetHeight(7.0);
  cylinderSource->SetResolution(100);

  // Create a mapper and actor
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(cylinderSource->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  //Create a renderer, render window, and interactor
  vtkNew<vtkOpenVRRenderer> renderer;
  vtkNew<vtkOpenVRRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("OpenVRCylinder");
  vtkNew<vtkOpenVRRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("ForestGreen").GetData());
  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
