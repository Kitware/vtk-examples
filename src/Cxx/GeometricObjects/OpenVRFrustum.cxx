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
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderer.h>
#include <vtkOpenVRRenderWindowInteractor.h>
#include <vtkShrinkPolyData.h>

int main(int, char *[])
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

  // Create a mapper and actor

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(shrink->GetOutputPort());

  vtkNew<vtkProperty> back;
  back->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->EdgeVisibilityOn();
  actor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
  actor->SetBackfaceProperty(back);

  // Create a renderer, render window, and interactor
  vtkNew<vtkOpenVRRenderer> renderer;
  vtkNew<vtkOpenVRRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("OpenVRFrustum");
  vtkNew<vtkOpenVRRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());

  // Position the camera so that we can see the frustum
  renderer->GetActiveCamera()->SetPosition(1, 0, 0);
  renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCamera();
  renderWindow->Render();
  renderWindowInteractor->Start();
  return EXIT_SUCCESS;
}
