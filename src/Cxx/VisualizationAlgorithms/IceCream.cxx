#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCone.h>
#include <vtkContourFilter.h>
#include <vtkImplicitBoolean.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>

int main(int, char*[])
{
  // This example demonstrates how to use boolean combinations of implicit
  // functions to create a model of an ice cream cone.

  vtkNew<vtkNamedColors> colors;

  // Create implicit function primitives. These have been carefully placed to
  // give the effect that we want. We are going to use various combinations of
  // these functions to create the shape we want; for example, we use planes
  // intersected with a cone (which is infinite in extent) to get a finite
  // cone.
  //
  vtkNew<vtkCone> cone;
  cone->SetAngle(20);

  vtkNew<vtkPlane> vertPlane;
  vertPlane->SetOrigin(.1, 0, 0);
  vertPlane->SetNormal(-1, 0, 0);

  vtkNew<vtkPlane> basePlane;
  basePlane->SetOrigin(1.2, 0, 0);
  basePlane->SetNormal(1, 0, 0);

  vtkNew<vtkSphere> iceCream;
  iceCream->SetCenter(1.333, 0, 0);
  iceCream->SetRadius(0.5);

  vtkNew<vtkSphere> bite;
  bite->SetCenter(1.5, 0, 0.5);
  bite->SetRadius(0.25);

  // Combine primitives to build ice-cream cone. Clip the cone with planes.
  vtkNew<vtkImplicitBoolean> theCone;
  theCone->SetOperationTypeToIntersection();
  theCone->AddFunction(cone);
  theCone->AddFunction(vertPlane);
  theCone->AddFunction(basePlane);

  // Take a bite out of the ice cream.
  vtkNew<vtkImplicitBoolean> theCream;
  theCream->SetOperationTypeToDifference();
  theCream->AddFunction(iceCream);
  theCream->AddFunction(bite);

  // The sample function generates a distance function from the
  // implicit function (which in this case is the cone). This is
  // then contoured to get a polygonal surface.
  //
  vtkNew<vtkSampleFunction> theConeSample;
  theConeSample->SetImplicitFunction(theCone);
  theConeSample->SetModelBounds(-1, 1.5, -1.25, 1.25, -1.25, 1.25);
  theConeSample->SetSampleDimensions(128, 128, 128);
  theConeSample->ComputeNormalsOff();

  vtkNew<vtkContourFilter> theConeSurface;
  theConeSurface->SetInputConnection(theConeSample->GetOutputPort());
  theConeSurface->SetValue(0, 0.0);

  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(theConeSurface->GetOutputPort());
  coneMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(colors->GetColor3d("chocolate").GetData());

  // The same here for the ice cream.
  //
  vtkNew<vtkSampleFunction> theCreamSample;
  theCreamSample->SetImplicitFunction(theCream);
  theCreamSample->SetModelBounds(0, 2.5, -1.25, 1.25, -1.25, 1.25);
  theCreamSample->SetSampleDimensions(128, 128, 128);
  theCreamSample->ComputeNormalsOff();

  vtkNew<vtkContourFilter> theCreamSurface;
  theCreamSurface->SetInputConnection(theCreamSample->GetOutputPort());
  theCreamSurface->SetValue(0, 0.0);

  vtkNew<vtkPolyDataMapper> creamMapper;
  creamMapper->SetInputConnection(theCreamSurface->GetOutputPort());
  creamMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> creamActor;
  creamActor->SetMapper(creamMapper);
  creamActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("mint").GetData());
  creamActor->GetProperty()->SetSpecular(.6);
  creamActor->GetProperty()->SetSpecularPower(50);

  // Create the usual rendering stuff
  //
  vtkNew<vtkRenderer> ren1;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(coneActor);
  ren1->AddActor(creamActor);
  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renWin->SetSize(640, 480);
  renWin->SetWindowName("IceCream");

  ren1->ResetCamera();
  ren1->GetActiveCamera()->Roll(90);
  ren1->GetActiveCamera()->Dolly(1.25);
  ren1->ResetCameraClippingRange();
  iren->Initialize();

  // render the image
  //
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
