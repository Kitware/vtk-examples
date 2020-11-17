// This example demonstrates the use of vtkVectorText and vtkFollower.
// vtkVectorText is used to create 3D annotation.  vtkFollower is used to
// position the 3D text and to ensure that the text always faces the
// renderer's active camera (i.e., the text is always readable).

#include <vtkActor.h>
#include <vtkAxes.h>
#include <vtkCamera.h>
#include <vtkFollower.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVectorText.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create the axes and the associated mapper and actor.
  vtkNew<vtkAxes> axes;
  axes->SetOrigin(0, 0, 0);
  vtkNew<vtkPolyDataMapper> axesMapper;
  axesMapper->SetInputConnection(axes->GetOutputPort());
  vtkNew<vtkActor> axesActor;
  axesActor->SetMapper(axesMapper);

  // Create the 3D text and the associated mapper and follower (a type of
  // actor).  Position the text so it is displayed over the origin of the
  // axes.
  vtkNew<vtkVectorText> atext;
  atext->SetText("Origin");
  vtkNew<vtkPolyDataMapper> textMapper;
  textMapper->SetInputConnection(atext->GetOutputPort());
  vtkNew<vtkFollower> textActor;
  textActor->SetMapper(textMapper);
  textActor->SetScale(0.2, 0.2, 0.2);
  textActor->AddPosition(0, -0.1, 0);
  textActor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  // Create the Renderer, RenderWindow, and RenderWindowInteractor.
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  interactor->SetInteractorStyle(style);

  // Add the actors to the renderer.
  renderer->AddActor(axesActor);
  renderer->AddActor(textActor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());

  // Zoom in closer.
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(1.6);

  // Reset the clipping range of the camera; set the camera of the
  // follower; render.
  renderer->ResetCameraClippingRange();
  textActor->SetCamera(renderer->GetActiveCamera());

  interactor->Initialize();
  renderWindow->SetWindowName("TextOrigin");
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
