#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGridOutlineFilter.h>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " combxyz.bin combq.bin" << std::endl;
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

  // create pipeline
  //
  vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
  pl3d->SetXYZFileName(argv[1]);
  pl3d->SetQFileName(argv[2]);
  pl3d->SetScalarFunctionNumber(100);
  pl3d->SetVectorFunctionNumber(202);
  pl3d->Update();

  vtkNew<vtkContourFilter> iso;
  iso->SetInputData(pl3d->GetOutput()->GetBlock(0));
  iso->SetValue(0, .38);

  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputConnection(iso->GetOutputPort());
  normals->SetFeatureAngle(45);

  vtkNew<vtkPolyDataMapper> isoMapper;
  isoMapper->SetInputConnection(normals->GetOutputPort());
  isoMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> isoActor;
  isoActor->SetMapper(isoMapper);
  isoActor->GetProperty()->SetColor(colors->GetColor3d("WhiteSmoke").GetData());

  vtkNew<vtkStructuredGridOutlineFilter> outline;
  outline->SetInputConnection(pl3d->GetOutputPort());

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(outlineActor);
  ren1->AddActor(isoActor);
  ren1->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  renWin->SetSize(640, 480);
  renWin->SetWindowName("CombustorIsosurface");

  ren1->GetActiveCamera()->SetFocalPoint(9.71821, 0.458166, 29.3999);
  ren1->GetActiveCamera()->SetPosition(2.7439, -37.3196, 38.7167);
  ren1->GetActiveCamera()->SetViewUp(-0.16123, 0.264271, 0.950876);
  ren1->GetActiveCamera()->Zoom(1.3);
  ren1->ResetCameraClippingRange();

  // render the image
  //
  renWin->Render();
  iren->Start();
  return EXIT_SUCCESS;
}
