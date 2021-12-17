#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderView.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  vtkNew<vtkRenderView> renderView;
  renderView->SetInteractionMode(vtkRenderView::INTERACTION_MODE_3D);
  renderView->GetRenderer()->AddActor(actor);
  renderView->RenderOnMouseMoveOn();
  renderView->Update();
  renderView->GetRenderer()->SetBackground(
      colors->GetColor3d("Beige").GetData());

  renderView->ResetCamera();
  renderView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
