#include <vtkBoundedPointSource.h>
#include <vtkCamera.h>
#include <vtkFitImplicitFunction.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphere.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  double radius = 1.0;
  vtkNew<vtkBoundedPointSource> points;
  points->SetNumberOfPoints(1000000);
  points->SetBounds(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);

  vtkNew<vtkSphere> sphere;
  sphere->SetRadius(radius);

  vtkNew<vtkFitImplicitFunction> fit;
  fit->SetInputConnection(points->GetOutputPort());
  fit->SetImplicitFunction(sphere);
  fit->SetThreshold(0.01);
  fit->Update();
  std::cout << fit->GetOutput()->GetNumberOfPoints() << " out of "
            << points->GetNumberOfPoints() << " points are within "
            << fit->GetThreshold() << " of the implicit function" << std::endl;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(radius * 0.05);

  vtkNew<vtkGlyph3D> glyph3D;
  glyph3D->SetInputConnection(fit->GetOutputPort());
  glyph3D->SetSourceConnection(sphereSource->GetOutputPort());
  glyph3D->ScalingOff();
  glyph3D->Update();

  vtkNew<vtkPolyDataMapper> glyph3DMapper;
  glyph3DMapper->SetInputConnection(glyph3D->GetOutputPort());

  vtkNew<vtkActor> glyph3DActor;
  glyph3DActor->SetMapper(glyph3DMapper);
  glyph3DActor->GetProperty()->SetColor(
      colors->GetColor3d("Banana").GetData());

  // Create graphics stuff
  //
  vtkNew<vtkRenderer> ren1;
  ren1->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetSize(512, 512);
  renWin->SetWindowName("FitImplicitFunction");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(glyph3DActor);

  // Generate an interesting view
  //
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Azimuth(120);
  ren1->GetActiveCamera()->Elevation(30);
  ren1->GetActiveCamera()->Dolly(1.0);
  ren1->ResetCameraClippingRange();

  renWin->Render();
  iren->Initialize();
  iren->Start();
  return EXIT_SUCCESS;
}
