#include <vtkActor.h>
#include <vtkContourFilter.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSuperquadric.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSuperquadric> implicitFunction;
  implicitFunction->SetPhiRoundness(2.5);
  implicitFunction->SetThetaRoundness(0.5);

  // Sample the function
  vtkNew<vtkSampleFunction> sample;
  sample->SetSampleDimensions(50, 50, 50);
  sample->SetImplicitFunction(implicitFunction);
  double value = 2.0;
  double xmin = -value, xmax = value, ymin = -value, ymax = value,
         zmin = -value, zmax = value;
  sample->SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax);

  // Create the 0 isosurface
  vtkNew<vtkContourFilter> contours;
  contours->SetInputConnection(sample->GetOutputPort());
  contours->GenerateValues(1, 2.0, 2.0);

  // Map the contours to graphical primitives
  vtkNew<vtkPolyDataMapper> contourMapper;
  contourMapper->SetInputConnection(contours->GetOutputPort());
  contourMapper->SetScalarRange(0.0, 1.2);

  // Create an actor for the contours
  vtkNew<vtkActor> contourActor;
  contourActor->SetMapper(contourMapper);

  // -- create a box around the function to indicate the sampling volume --

  // Create outline
  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(sample->GetOutputPort());

  // Map it to graphics primitives
  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  // Create an actor for it
  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Samplefunction");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(contourActor);
  renderer->AddActor(outlineActor);
  renderer->SetBackground(colors->GetColor3d("Tan").GetData());

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
