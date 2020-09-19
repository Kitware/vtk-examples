#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkFrustumSource.h>
#include <vtkMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlanes.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShrinkPolyData.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkCamera> camera;
  camera->SetClippingRange(.1, .4);
  double planesArray[24];

  camera->GetFrustumPlanes(1.0, planesArray);

  vtkNew<vtkPlanes> planes;
  planes->SetFrustumPlanes(planesArray);

  vtkNew<vtkFrustumSource> frustumSource;
  frustumSource->ShowLinesOff();
  frustumSource->SetPlanes(planes);

  vtkNew<vtkShrinkPolyData> shrink;
  shrink->SetInputConnection(frustumSource->GetOutputPort());
  shrink->SetShrinkFactor(.9);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(shrink->GetOutputPort());

  vtkNew<vtkProperty> back;
  back->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->EdgeVisibilityOn();
  actor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
  actor->SetBackfaceProperty(back);

  // a renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("Frustum");
  renderWindow->AddRenderer(renderer);

  // an interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());

  // Position the camera so that we can see the frustum
  renderer->GetActiveCamera()->SetPosition(1, 0, 0);
  renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCamera();

  // render an image (lights and cameras are created automatically)
  renderWindow->Render();

  // begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
