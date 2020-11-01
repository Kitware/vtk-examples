#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkQuadric.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create the quadric function definition
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(.5, 1, .2, 0, .1, 0, 0, .2, 0, 0);

  // Sample the quadric function
  vtkNew<vtkSampleFunction> sample;
  sample->SetSampleDimensions(50, 50, 50);
  sample->SetImplicitFunction(quadric);
  double xmin = 0, xmax = 1, ymin = 0, ymax = 1, zmin = 0, zmax = 1;
  sample->SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax);

  vtkNew<vtkContourFilter> contourFilter;
  contourFilter->SetInputConnection(sample->GetOutputPort());
  contourFilter->GenerateValues(1, 1.0, 1.0);
  contourFilter->Update();

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(contourFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("QuadraticSurface");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("LimeGreen").GetData());

  renderWindow->Render();

  renderer->GetActiveCamera()->Elevation(-45);
  renderer->GetActiveCamera()->Azimuth(45);
  renderer->ResetCamera();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
