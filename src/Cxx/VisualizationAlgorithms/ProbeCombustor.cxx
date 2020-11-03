#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

// This shows how to probe a dataset with a plane. The probed data is then
// contoured.

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " combxyz.bin combq.bin" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  // create pipeline
  //
  vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
  pl3d->SetXYZFileName(argv[1]);
  pl3d->SetQFileName(argv[2]);
  pl3d->SetScalarFunctionNumber(100);
  pl3d->SetVectorFunctionNumber(202);
  pl3d->Update();

  vtkStructuredGrid* sg =
      dynamic_cast<vtkStructuredGrid*>(pl3d->GetOutput()->GetBlock(0));

  // We create three planes and position them in the correct position
  // using transform filters. They are then appended together and used as
  // a probe.
  vtkNew<vtkPlaneSource> plane;
  plane->SetResolution(50, 50);

  vtkNew<vtkTransform> transP1;
  transP1->Translate(3.7, 0.0, 28.37);
  transP1->Scale(5, 5, 5);
  transP1->RotateY(90);

  vtkNew<vtkTransformPolyDataFilter> tpd1;
  tpd1->SetInputConnection(plane->GetOutputPort());
  tpd1->SetTransform(transP1);

  vtkNew<vtkOutlineFilter> outTpd1;
  outTpd1->SetInputConnection(tpd1->GetOutputPort());

  vtkNew<vtkPolyDataMapper> mapTpd1;
  mapTpd1->SetInputConnection(outTpd1->GetOutputPort());

  vtkNew<vtkActor> tpd1Actor;
  tpd1Actor->SetMapper(mapTpd1);
  tpd1Actor->GetProperty()->SetColor(0, 0, 0);
  tpd1Actor->GetProperty()->SetLineWidth(2.0);

  vtkNew<vtkTransform> transP2;
  transP2->Translate(9.2, 0.0, 31.20);
  transP2->Scale(5, 5, 5);
  transP2->RotateY(90);

  vtkNew<vtkTransformPolyDataFilter> tpd2;
  tpd2->SetInputConnection(plane->GetOutputPort());
  tpd2->SetTransform(transP2);

  vtkNew<vtkOutlineFilter> outTpd2;
  outTpd2->SetInputConnection(tpd2->GetOutputPort());

  vtkNew<vtkPolyDataMapper> mapTpd2;
  mapTpd2->SetInputConnection(outTpd2->GetOutputPort());

  vtkNew<vtkActor> tpd2Actor;
  tpd2Actor->SetMapper(mapTpd2);
  tpd2Actor->GetProperty()->SetColor(0, 0, 0);
  tpd2Actor->GetProperty()->SetLineWidth(2.0);

  vtkNew<vtkTransform> transP3;
  transP3->Translate(13.27, 0.0, 33.30);
  transP3->Scale(5, 5, 5);
  transP3->RotateY(90);

  vtkNew<vtkTransformPolyDataFilter> tpd3;
  tpd3->SetInputConnection(plane->GetOutputPort());
  tpd3->SetTransform(transP3);

  vtkNew<vtkOutlineFilter> outTpd3;
  outTpd3->SetInputConnection(tpd3->GetOutputPort());

  vtkNew<vtkPolyDataMapper> mapTpd3;
  mapTpd3->SetInputConnection(outTpd3->GetOutputPort());

  vtkNew<vtkActor> tpd3Actor;
  tpd3Actor->SetMapper(mapTpd3);
  tpd3Actor->GetProperty()->SetColor(0, 0, 0);
  tpd3Actor->GetProperty()->SetLineWidth(2.0);

  vtkNew<vtkAppendPolyData> appendF;
  appendF->AddInputConnection(tpd1->GetOutputPort());
  appendF->AddInputConnection(tpd2->GetOutputPort());
  appendF->AddInputConnection(tpd3->GetOutputPort());

  // The vtkProbeFilter takes two inputs. One is a dataset to use as the probe
  // geometry (SetInputConnection); the other is the data to probe
  // (SetSourceConnection). The output dataset structure (geometry and
  // topology) of the probe is the same as the structure of the input. The
  // probing process generates new data values resampled from the source.
  vtkNew<vtkProbeFilter> probe;
  probe->SetInputConnection(appendF->GetOutputPort());
  probe->SetSourceData(sg);

  vtkNew<vtkContourFilter> contour;
  contour->SetInputConnection(probe->GetOutputPort());
  contour->GenerateValues(50, sg->GetScalarRange());

  vtkNew<vtkPolyDataMapper> contourMapper;
  contourMapper->SetInputConnection(contour->GetOutputPort());
  contourMapper->SetScalarRange(sg->GetScalarRange());

  vtkNew<vtkActor> planeActor;
  planeActor->SetMapper(contourMapper);

  vtkNew<vtkStructuredGridOutlineFilter> outline;
  outline->SetInputData(sg);

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(0, 0, 0);
  outlineActor->GetProperty()->SetLineWidth(2.0);

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkRenderer> ren1;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  ren1->AddActor(outlineActor);
  ren1->AddActor(planeActor);
  ren1->AddActor(tpd1Actor);
  ren1->AddActor(tpd2Actor);
  ren1->AddActor(tpd3Actor);
  ren1->SetBackground(colors->GetColor3d("Gainsboro").GetData());
  renWin->SetSize(640, 480);
  renWin->SetWindowName("ProbeCombustor");

  ren1->ResetCamera();
  ren1->GetActiveCamera()->SetClippingRange(3.95297, 50);
  ren1->GetActiveCamera()->SetFocalPoint(8.88908, 0.595038, 29.3342);
  ren1->GetActiveCamera()->SetPosition(-12.3332, 31.7479, 41.2387);
  ren1->GetActiveCamera()->SetViewUp(0.060772, -0.319905, 0.945498);

  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
