#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkClipPolyData.h>
#include <vtkCylinder.h>
#include <vtkImplicitBoolean.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphere.h>
#include <vtkTransform.h>

int main(int, char*[])
{
  // Demonstrate the use of clipping on polygonal data
  //

  // create pipeline
  //
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPlaneSource> plane;
  plane->SetXResolution(25);
  plane->SetYResolution(25);
  plane->SetOrigin(-1, -1, 0);
  plane->SetPoint1(1, -1, 0);
  plane->SetPoint2(-1, 1, 0);

  vtkNew<vtkTransform> transformSphere;
  transformSphere->Identity();
  transformSphere->Translate(.4, -.4, 0);
  transformSphere->Inverse();

  vtkNew<vtkSphere> sphere;
  sphere->SetTransform(transformSphere);
  sphere->SetRadius(.5);

  vtkNew<vtkTransform> transformCylinder;
  transformCylinder->Identity();
  transformCylinder->Translate(-.4, .4, 0);
  transformCylinder->RotateZ(30);
  transformCylinder->RotateY(60);
  transformCylinder->RotateX(90);
  transformCylinder->Inverse();

  vtkNew<vtkCylinder> cylinder;
  cylinder->SetTransform(transformCylinder);
  cylinder->SetRadius(.3);

  vtkNew<vtkImplicitBoolean> boolean;
  boolean->AddFunction(cylinder);
  boolean->AddFunction(sphere);

  vtkNew<vtkClipPolyData> clipper;
  clipper->SetInputConnection(plane->GetOutputPort());
  clipper->SetClipFunction(boolean);
  clipper->GenerateClippedOutputOn();
  clipper->GenerateClipScalarsOn();
  clipper->SetValue(0);

  vtkNew<vtkPolyDataMapper> clipMapper;
  clipMapper->SetInputConnection(clipper->GetOutputPort());
  clipMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> clipActor;
  clipActor->SetMapper(clipMapper);
  clipActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("MidnightBlue").GetData());
  clipActor->GetProperty()->SetRepresentationToWireframe();

  vtkNew<vtkPolyDataMapper> clipInsideMapper;
  clipInsideMapper->SetInputData(clipper->GetClippedOutput());
  clipInsideMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> clipInsideActor;
  clipInsideActor->SetMapper(clipInsideMapper);
  clipInsideActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("LightBlue").GetData());

  // Create graphics stuff
  //
  vtkNew<vtkRenderer> ren1;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(clipActor);

  ren1->AddActor(clipInsideActor);
  ren1->SetBackground(colors->GetColor3d("Wheat").GetData());
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Dolly(1.4);
  ren1->ResetCameraClippingRange();

  renWin->SetSize(640, 480);
  renWin->SetWindowName("ClipSphereCylinder");

  // render the image
  //
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
