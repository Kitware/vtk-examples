#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCutter.h>
#include <vtkDataSetMapper.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " combxyz.bin combq.bin" << std::endl;
    return EXIT_FAILURE;
  }

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> ren1;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // cut data
  vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
  pl3d->SetXYZFileName(argv[1]);
  pl3d->SetQFileName(argv[2]);
  pl3d->SetScalarFunctionNumber(100);
  pl3d->SetVectorFunctionNumber(202);
  pl3d->Update();

  vtkStructuredGrid* sg =
      dynamic_cast<vtkStructuredGrid*>(pl3d->GetOutput()->GetBlock(0));

  vtkNew<vtkPlane> plane;
  plane->SetOrigin(sg->GetCenter());
  plane->SetNormal(-0.287, 0, 0.9579);

  vtkNew<vtkCutter> planeCut;
  planeCut->SetInputData(pl3d->GetOutput()->GetBlock(0));
  planeCut->SetCutFunction(plane);

  vtkNew<vtkDataSetMapper> cutMapper;
  cutMapper->SetInputConnection(planeCut->GetOutputPort());
  cutMapper->SetScalarRange(sg->GetPointData()->GetScalars()->GetRange());

  vtkNew<vtkActor> cutActor;
  cutActor->SetMapper(cutMapper);

  // extract plane
  vtkNew<vtkStructuredGridGeometryFilter> compPlane;
  compPlane->SetInputData(sg);
  compPlane->SetExtent(0, 100, 0, 100, 9, 9);

  vtkNew<vtkPolyDataMapper> planeMapper;
  planeMapper->SetInputConnection(compPlane->GetOutputPort());
  planeMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> planeActor;
  planeActor->SetMapper(planeMapper);
  planeActor->GetProperty()->SetRepresentationToWireframe();
  planeActor->GetProperty()->SetColor(colors->GetColor3d("Wheat").GetData());

  // outline
  vtkNew<vtkStructuredGridOutlineFilter> outline;
  outline->SetInputData(pl3d->GetOutput()->GetBlock(0));

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("Wheat").GetData());

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(outlineActor);
  ren1->AddActor(planeActor);
  ren1->AddActor(cutActor);
  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renWin->SetSize(640, 480);
  renWin->SetWindowName("CutStructuredGrid");

  auto camera = ren1->GetActiveCamera();
  camera->SetPosition(5.02611, -23.535, 50.3979);
  camera->SetFocalPoint(9.33614, 0.0414149, 30.112);
  camera->SetViewUp(-0.0676794, 0.657814, 0.750134);
  camera->SetDistance(31.3997);
  camera->SetClippingRange(12.1468, 55.8147);

  // render the image
  //
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
