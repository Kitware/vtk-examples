#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkGaussianSplatter.h>
#include <vtkMaskPoints.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " fran_cut.vtk" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // read cyberware file
  //
  vtkNew<vtkPolyDataReader> cyber;
  cyber->SetFileName(argv[1]);

  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputConnection(cyber->GetOutputPort());

  vtkNew<vtkMaskPoints> mask;
  mask->SetInputConnection(normals->GetOutputPort());
  mask->SetOnRatio(8);
  //   mask->RandomModeOn();

  vtkNew<vtkGaussianSplatter> splatter;
  splatter->SetInputConnection(mask->GetOutputPort());
  splatter->SetSampleDimensions(100, 100, 100);
  splatter->SetEccentricity(2.5);
  splatter->NormalWarpingOn();
  splatter->SetScaleFactor(1.0);
  splatter->SetRadius(0.025);

  vtkNew<vtkContourFilter> contour;
  contour->SetInputConnection(splatter->GetOutputPort());
  contour->SetValue(0, 0.25);

  vtkNew<vtkPolyDataMapper> splatMapper;
  splatMapper->SetInputConnection(contour->GetOutputPort());
  splatMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> splatActor;
  splatActor->SetMapper(splatMapper);
  splatActor->GetProperty()->SetColor(colors->GetColor3d("Flesh").GetData());

  vtkNew<vtkPolyDataMapper> cyberMapper;
  cyberMapper->SetInputConnection(cyber->GetOutputPort());
  cyberMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> cyberActor;
  cyberActor->SetMapper(cyberMapper);
  cyberActor->GetProperty()->SetRepresentationToWireframe();
  cyberActor->GetProperty()->SetColor(
      colors->GetColor3d("Turquoise").GetData());

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(cyberActor);
  ren1->AddActor(splatActor);
  ren1->SetBackground(colors->GetColor3d("Wheat").GetData());
  renWin->SetSize(640, 480);
  renWin->SetWindowName("SplatFace");

  vtkNew<vtkCamera> camera;
  camera->SetClippingRange(0.0332682, 1.66341);
  camera->SetFocalPoint(0.0511519, -0.127555, -0.0554379);
  camera->SetPosition(0.516567, -0.124763, -0.349538);
  camera->SetViewAngle(18.1279);
  camera->SetViewUp(-0.013125, 0.99985, -0.0112779);
  ren1->SetActiveCamera(camera);

  // render the image
  //
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
