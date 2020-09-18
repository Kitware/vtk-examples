#include <vtkActor.h>
#include <vtkColor.h>
#include <vtkContourFilter.h>
#include <vtkImageData.h>
#include <vtkImplicitBoolean.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>

int main(int, char*[])
{
  // Define colors
  vtkNew<vtkNamedColors> colors;
  vtkColor3d background = colors->GetColor3d("SlateGray");
  vtkColor3d actorColor = colors->GetColor3d("hot_pink");

  vtkNew<vtkSphere> sphere1;
  sphere1->SetCenter(0.9, 0, 0);
  vtkNew<vtkSphere> sphere2;
  sphere2->SetCenter(-0.9, 0, 0);

  vtkNew<vtkImplicitBoolean> implicitBoolean;
  implicitBoolean->AddFunction(sphere1);
  implicitBoolean->AddFunction(sphere2);
  implicitBoolean->SetOperationTypeToUnion();

  //  implicitBoolean->SetOperationTypeToIntersection();
  //  implicitBoolean->SetOperationTypeToDifference();

  // Sample the function
  vtkNew<vtkSampleFunction> sample;
  sample->SetSampleDimensions(50, 50, 50);
  sample->SetImplicitFunction(implicitBoolean);
  double value = 3.0;
  double xmin = -value, xmax = value, ymin = -value, ymax = value,
         zmin = -value, zmax = value;
  sample->SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax);

  // Create the 0 isosurface
  vtkNew<vtkContourFilter> contours;
  contours->SetInputConnection(sample->GetOutputPort());
  contours->GenerateValues(1, 1, 1);

  // Map the contours to graphical primitives
  vtkNew<vtkPolyDataMapper> contourMapper;
  contourMapper->SetInputConnection(contours->GetOutputPort());
  contourMapper->ScalarVisibilityOff();

  // Create an actor for the contours
  vtkNew<vtkActor> contourActor;
  contourActor->SetMapper(contourMapper);
  contourActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("hot_pink").GetData());
  contourActor->GetProperty()->SetDiffuse(.8);
  contourActor->GetProperty()->SetSpecular(.2);
  contourActor->GetProperty()->SetSpecularPower(60.0);

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("ImplicitBoolean");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(contourActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
