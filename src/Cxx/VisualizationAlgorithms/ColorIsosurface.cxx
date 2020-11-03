// This example shows how to color an isosurface with other
// data. Basically an isosurface is generated, and a data array is
// selected and used by the mapper to color the surface.

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkLODActor.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
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
  // Read some data. The important thing here is to read a function as a
  // data array as well as the scalar and vector.  (here function 153 is
  // named "Velocity Magnitude").Later this data array will be used to
  // color the isosurface.
  vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
  pl3d->SetXYZFileName(argv[1]);
  pl3d->SetQFileName(argv[2]);
  pl3d->SetScalarFunctionNumber(100);
  pl3d->SetVectorFunctionNumber(202);
  pl3d->AddFunction(153);
  pl3d->Update();

  // The contour filter uses the labeled scalar (function number 100
  // above to generate the contour surface; all other data is
  // interpolated during the contouring process.
  vtkNew<vtkContourFilter> iso;
  iso->SetInputData(pl3d->GetOutput()->GetBlock(0));
  iso->SetValue(0, .24);

  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputConnection(iso->GetOutputPort());
  normals->SetFeatureAngle(45);

  // We indicate to the mapper to use the velcoity magnitude, which is a
  // vtkDataArray that makes up part of the point attribute data.
  vtkNew<vtkPolyDataMapper> isoMapper;
  isoMapper->SetInputConnection(normals->GetOutputPort());
  isoMapper->ScalarVisibilityOn();
  isoMapper->SetScalarRange(0, 1500);
  isoMapper->SetScalarModeToUsePointFieldData();
  isoMapper->ColorByArrayComponent("VelocityMagnitude", 0);

  vtkNew<vtkLODActor> isoActor;
  isoActor->SetMapper(isoMapper);
  isoActor->SetNumberOfCloudPoints(1000);

  vtkNew<vtkStructuredGridOutlineFilter> outline;
  outline->SetInputData(pl3d->GetOutput()->GetBlock(0));

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);

  // Create the usual rendering stuff.
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkNamedColors> colors;

  // Add the actors to the renderer, set the background and size
  renderer->AddActor(outlineActor);
  renderer->AddActor(isoActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("ColorIsosurface");

  // Pick a good view
  vtkCamera* cam1 = renderer->GetActiveCamera();
  cam1->SetClippingRange(3.95297, 50);
  cam1->SetFocalPoint(9.71821, 0.458166, 29.3999);
  cam1->SetPosition(2.7439, -37.3196, 38.7167);
  cam1->SetViewUp(-0.16123, 0.264271, 0.950876);
  cam1->Zoom(1.3);

  interactor->Initialize();
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
