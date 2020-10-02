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
#include <vtkQuadric.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>

#include <cstdlib>

int main(int argc, char* argv[])
{
  // Define colors
  vtkNew<vtkNamedColors> colors;
  vtkColor3d actorColor = colors->GetColor3d("AliceBlue");
  vtkColor3d EdgeColour = colors->GetColor3d("SteelBlue");
  vtkColor3d BackgroundColour = colors->GetColor3d("Silver");

  // create a Quadric
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(0.5, 1, 0.2, 0, 0.1, 0, 0, 0.2, 0, 0);

  /*The sample function generates a distance function from the implicit
        function.This is then contoured to get a polygonal surface.*/
  vtkNew<vtkSampleFunction> sample;
  sample->SetImplicitFunction(quadric);
  sample->SetModelBounds(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5);
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
  renderWindow->SetWindowName("ImplicitQuadric");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  // add the actor
  renderer->AddActor(actor);
  // Start
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
