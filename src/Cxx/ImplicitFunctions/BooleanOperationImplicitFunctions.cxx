#include <vtkActor.h>
#include <vtkBox.h>
#include <vtkCamera.h>
#include <vtkColor.h>
#include <vtkContourFilter.h>
#include <vtkImplicitBoolean.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>

int main(int argc, char* argv[])
{
  // Define colors
  vtkNew<vtkNamedColors> colors;
  vtkColor3d actorColor = colors->GetColor3d("AliceBlue");
  vtkColor3d EdgeColour = colors->GetColor3d("SteelBlue");
  vtkColor3d BackgroundColour = colors->GetColor3d("Silver");

  // create a sphere
  vtkNew<vtkSphere> sphere;
  sphere->SetCenter(1.0, 0.0, 0.0);
  sphere->SetRadius(1);

  // create a box
  vtkNew<vtkBox> box;
  box->SetBounds(-1, 1, -1, 1, -1, 1);

  // combine the two implicit functions
  vtkNew<vtkImplicitBoolean> boolean;
  boolean->SetOperationTypeToDifference();
  // boolean->SetOperationTypeToUnion()
  // boolean->SetOperationTypeToIntersection()
  boolean->AddFunction(box);
  boolean->AddFunction(sphere);
  // The sample function generates a distance function from the implicit
  // function.This is then contoured to get a polygonal surface.
  vtkNew<vtkSampleFunction> sample;
  sample->SetImplicitFunction(boolean);
  sample->SetModelBounds(-1, 2, -1, 1, -1, 1);
  sample->SetSampleDimensions(40, 40, 40);
  sample->ComputeNormalsOff();

  // contour
  vtkNew<vtkContourFilter> surface;
  surface->SetInputConnection(sample->GetOutputPort());
  surface->SetValue(0, 0.0);

  // Create a mapper and an actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(surface->GetOutputPort());
  mapper->ScalarVisibilityOff();
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->EdgeVisibilityOn();
  actor->GetProperty()->SetColor(actorColor.GetData());
  actor->GetProperty()->SetEdgeColor(EdgeColour.GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(BackgroundColour.GetData());
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("BooleanOperationImplicitFunctions");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // add the actor
  renderer->AddActor(actor);

  // Start
  renderer->GetActiveCamera()->SetPosition(5.0, -4.0, 1.6);
  renderer->GetActiveCamera()->SetViewUp(0.1, 0.5, 0.9);
  renderer->GetActiveCamera()->SetDistance(6.7);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
