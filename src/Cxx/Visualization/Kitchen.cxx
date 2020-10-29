#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkLineSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStreamTracer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkStructuredGridReader.h>

#include <array>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " kitchen.vtk" << std::endl;
    return EXIT_FAILURE;
  }
  double range[2];
  vtkNew<vtkNamedColors> colors;

  // Set the furniture colors.
  std::array<unsigned char, 4> furnColor{{204, 204, 153, 255}};
  colors->SetColor("Furniture", furnColor.data());

  vtkNew<vtkRenderer> aren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(aren);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);
  //
  // Read data
  //
  vtkNew<vtkStructuredGridReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update(); // force a read to occur
  reader->GetOutput()->GetLength();

  double maxTime = 0.0;

  if (reader->GetOutput()->GetPointData()->GetScalars())
  {
    reader->GetOutput()->GetPointData()->GetScalars()->GetRange(range);
  }
  if (reader->GetOutput()->GetPointData()->GetVectors())
  {
    auto maxVelocity =
        reader->GetOutput()->GetPointData()->GetVectors()->GetMaxNorm();
    maxTime = 4.0 * reader->GetOutput()->GetLength() / maxVelocity;
  }
  //
  // Outline around data
  //
  vtkNew<vtkStructuredGridOutlineFilter> outlineF;
  outlineF->SetInputConnection(reader->GetOutputPort());
  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outlineF->GetOutputPort());
  vtkNew<vtkActor> outline;
  outline->SetMapper(outlineMapper);
  outline->GetProperty()->SetColor(colors->GetColor3d("LampBlack").GetData());
  //
  // Set up shaded surfaces (i.e., supporting geometry)
  //
  vtkNew<vtkStructuredGridGeometryFilter> doorGeom;
  doorGeom->SetInputConnection(reader->GetOutputPort());
  doorGeom->SetExtent(27, 27, 14, 18, 0, 11);
  vtkNew<vtkPolyDataMapper> mapDoor;
  mapDoor->SetInputConnection(doorGeom->GetOutputPort());
  mapDoor->ScalarVisibilityOff();
  vtkNew<vtkActor> door;
  door->SetMapper(mapDoor);
  door->GetProperty()->SetColor(colors->GetColor3d("Burlywood").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> window1Geom;
  window1Geom->SetInputConnection(reader->GetOutputPort());
  window1Geom->SetExtent(0, 0, 9, 18, 6, 12);
  vtkNew<vtkPolyDataMapper> mapWindow1;
  mapWindow1->SetInputConnection(window1Geom->GetOutputPort());
  mapWindow1->ScalarVisibilityOff();
  vtkNew<vtkActor> window1;
  window1->SetMapper(mapWindow1);
  window1->GetProperty()->SetColor(colors->GetColor3d("SkyBlue").GetData());
  window1->GetProperty()->SetOpacity(.6);

  vtkNew<vtkStructuredGridGeometryFilter> window2Geom;
  window2Geom->SetInputConnection(reader->GetOutputPort());
  window2Geom->SetExtent(5, 12, 23, 23, 6, 12);
  vtkNew<vtkPolyDataMapper> mapWindow2;
  mapWindow2->SetInputConnection(window2Geom->GetOutputPort());
  mapWindow2->ScalarVisibilityOff();
  vtkNew<vtkActor> window2;
  window2->SetMapper(mapWindow2);
  window2->GetProperty()->SetColor(colors->GetColor3d("SkyBlue").GetData());
  window2->GetProperty()->SetOpacity(.6);

  vtkNew<vtkStructuredGridGeometryFilter> klower1Geom;
  klower1Geom->SetInputConnection(reader->GetOutputPort());
  klower1Geom->SetExtent(17, 17, 0, 11, 0, 6);
  vtkNew<vtkPolyDataMapper> mapKlower1;
  mapKlower1->SetInputConnection(klower1Geom->GetOutputPort());
  mapKlower1->ScalarVisibilityOff();
  vtkNew<vtkActor> klower1;
  klower1->SetMapper(mapKlower1);
  klower1->GetProperty()->SetColor(colors->GetColor3d("EggShell").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> klower2Geom;
  klower2Geom->SetInputConnection(reader->GetOutputPort());
  klower2Geom->SetExtent(19, 19, 0, 11, 0, 6);
  vtkNew<vtkPolyDataMapper> mapKlower2;
  mapKlower2->SetInputConnection(klower2Geom->GetOutputPort());
  mapKlower2->ScalarVisibilityOff();
  vtkNew<vtkActor> klower2;
  klower2->SetMapper(mapKlower2);
  klower2->GetProperty()->SetColor(colors->GetColor3d("EggShell").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> klower3Geom;
  klower3Geom->SetInputConnection(reader->GetOutputPort());
  klower3Geom->SetExtent(17, 19, 0, 0, 0, 6);
  vtkNew<vtkPolyDataMapper> mapKlower3;
  mapKlower3->SetInputConnection(klower3Geom->GetOutputPort());
  mapKlower3->ScalarVisibilityOff();
  vtkNew<vtkActor> klower3;
  klower3->SetMapper(mapKlower3);
  klower3->GetProperty()->SetColor(colors->GetColor3d("EggShell").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> klower4Geom;
  klower4Geom->SetInputConnection(reader->GetOutputPort());
  klower4Geom->SetExtent(17, 19, 11, 11, 0, 6);
  vtkNew<vtkPolyDataMapper> mapKlower4;
  mapKlower4->SetInputConnection(klower4Geom->GetOutputPort());
  mapKlower4->ScalarVisibilityOff();
  vtkNew<vtkActor> klower4;
  klower4->SetMapper(mapKlower4);
  klower4->GetProperty()->SetColor(colors->GetColor3d("EggShell").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> klower5Geom;
  klower5Geom->SetInputConnection(reader->GetOutputPort());
  klower5Geom->SetExtent(17, 19, 0, 11, 0, 0);
  vtkNew<vtkPolyDataMapper> mapKlower5;
  mapKlower5->SetInputConnection(klower5Geom->GetOutputPort());
  mapKlower5->ScalarVisibilityOff();
  vtkNew<vtkActor> klower5;
  klower5->SetMapper(mapKlower5);
  klower5->GetProperty()->SetColor(colors->GetColor3d("EggShell").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> klower6Geom;
  klower6Geom->SetInputConnection(reader->GetOutputPort());
  klower6Geom->SetExtent(17, 19, 0, 7, 6, 6);
  vtkNew<vtkPolyDataMapper> mapKlower6;
  mapKlower6->SetInputConnection(klower6Geom->GetOutputPort());
  mapKlower6->ScalarVisibilityOff();
  vtkNew<vtkActor> klower6;
  klower6->SetMapper(mapKlower6);
  klower6->GetProperty()->SetColor(colors->GetColor3d("EggShell").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> klower7Geom;
  klower7Geom->SetInputConnection(reader->GetOutputPort());
  klower7Geom->SetExtent(17, 19, 9, 11, 6, 6);
  vtkNew<vtkPolyDataMapper> mapKlower7;
  mapKlower7->SetInputConnection(klower7Geom->GetOutputPort());
  mapKlower7->ScalarVisibilityOff();
  vtkNew<vtkActor> klower7;
  klower7->SetMapper(mapKlower7);
  klower7->GetProperty()->SetColor(colors->GetColor3d("EggShell").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> hood1Geom;
  hood1Geom->SetInputConnection(reader->GetOutputPort());
  hood1Geom->SetExtent(17, 17, 0, 11, 11, 16);
  vtkNew<vtkPolyDataMapper> mapHood1;
  mapHood1->SetInputConnection(hood1Geom->GetOutputPort());
  mapHood1->ScalarVisibilityOff();
  vtkNew<vtkActor> hood1;
  hood1->SetMapper(mapHood1);
  hood1->GetProperty()->SetColor(colors->GetColor3d("Silver").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> hood2Geom;
  hood2Geom->SetInputConnection(reader->GetOutputPort());
  hood2Geom->SetExtent(19, 19, 0, 11, 11, 16);
  vtkNew<vtkPolyDataMapper> mapHood2;
  mapHood2->SetInputConnection(hood2Geom->GetOutputPort());
  mapHood2->ScalarVisibilityOff();
  vtkNew<vtkActor> hood2;
  hood2->SetMapper(mapHood2);
  hood2->GetProperty()->SetColor(colors->GetColor3d("Furniture").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> hood3Geom;
  hood3Geom->SetInputConnection(reader->GetOutputPort());
  hood3Geom->SetExtent(17, 19, 0, 0, 11, 16);
  vtkNew<vtkPolyDataMapper> mapHood3;
  mapHood3->SetInputConnection(hood3Geom->GetOutputPort());
  mapHood3->ScalarVisibilityOff();
  vtkNew<vtkActor> hood3;
  hood3->SetMapper(mapHood3);
  hood3->GetProperty()->SetColor(colors->GetColor3d("Furniture").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> hood4Geom;
  hood4Geom->SetInputConnection(reader->GetOutputPort());
  hood4Geom->SetExtent(17, 19, 11, 11, 11, 16);
  vtkNew<vtkPolyDataMapper> mapHood4;
  mapHood4->SetInputConnection(hood4Geom->GetOutputPort());
  mapHood4->ScalarVisibilityOff();
  vtkNew<vtkActor> hood4;
  hood4->SetMapper(mapHood4);
  hood4->GetProperty()->SetColor(colors->GetColor3d("Furniture").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> hood6Geom;
  hood6Geom->SetInputConnection(reader->GetOutputPort());
  hood6Geom->SetExtent(17, 19, 0, 11, 16, 16);
  vtkNew<vtkPolyDataMapper> mapHood6;
  mapHood6->SetInputConnection(hood6Geom->GetOutputPort());
  mapHood6->ScalarVisibilityOff();
  vtkNew<vtkActor> hood6;
  hood6->SetMapper(mapHood6);
  hood6->GetProperty()->SetColor(colors->GetColor3d("Furniture").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> cookingPlateGeom;
  cookingPlateGeom->SetInputConnection(reader->GetOutputPort());
  cookingPlateGeom->SetExtent(17, 19, 7, 9, 6, 6);
  vtkNew<vtkPolyDataMapper> mapCookingPlate;
  mapCookingPlate->SetInputConnection(cookingPlateGeom->GetOutputPort());
  mapCookingPlate->ScalarVisibilityOff();
  vtkNew<vtkActor> cookingPlate;
  cookingPlate->SetMapper(mapCookingPlate);
  cookingPlate->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkStructuredGridGeometryFilter> filterGeom;
  filterGeom->SetInputConnection(reader->GetOutputPort());
  filterGeom->SetExtent(17, 19, 7, 9, 11, 11);
  vtkNew<vtkPolyDataMapper> mapFilter;
  mapFilter->SetInputConnection(filterGeom->GetOutputPort());
  mapFilter->ScalarVisibilityOff();
  vtkNew<vtkActor> filter;
  filter->SetMapper(mapFilter);
  filter->GetProperty()->SetColor(colors->GetColor3d("Furniture").GetData());
  //
  // regular streamlines
  //
  vtkNew<vtkLineSource> line;
  line->SetResolution(39);
  line->SetPoint1(0.08, 2.50, 0.71);
  line->SetPoint2(0.08, 4.50, 0.71);
  vtkNew<vtkPolyDataMapper> rakeMapper;
  rakeMapper->SetInputConnection(line->GetOutputPort());
  vtkNew<vtkActor> rake;
  rake->SetMapper(rakeMapper);

  vtkNew<vtkStreamTracer> streamers;
  // streamers->DebugOn();
  streamers->SetInputConnection(reader->GetOutputPort());
  streamers->SetSourceConnection(line->GetOutputPort());
  streamers->SetMaximumPropagation(maxTime);
  streamers->SetInitialIntegrationStep(.5);
  streamers->SetMinimumIntegrationStep(.1);
  streamers->SetIntegratorType(2);
  streamers->Update();

  vtkNew<vtkPolyDataMapper> streamersMapper;
  streamersMapper->SetInputConnection(streamers->GetOutputPort());
  streamersMapper->SetScalarRange(range);

  vtkNew<vtkActor> lines;
  lines->SetMapper(streamersMapper);
  lines->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  aren->TwoSidedLightingOn();

  aren->AddActor(outline);
  aren->AddActor(door);
  aren->AddActor(window1);
  aren->AddActor(window2);
  aren->AddActor(klower1);
  aren->AddActor(klower2);
  aren->AddActor(klower3);
  aren->AddActor(klower4);
  aren->AddActor(klower5);
  aren->AddActor(klower6);
  aren->AddActor(klower7);
  aren->AddActor(hood1);
  aren->AddActor(hood2);
  aren->AddActor(hood3);
  aren->AddActor(hood4);
  aren->AddActor(hood6);
  aren->AddActor(cookingPlate);
  aren->AddActor(filter);
  aren->AddActor(lines);
  aren->AddActor(rake);

  aren->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkCamera> aCamera;
  aren->SetActiveCamera(aCamera);
  aren->ResetCamera();

  aCamera->SetFocalPoint(3.505, 2.505, 1.255);
  aCamera->SetPosition(3.505, 24.6196, 1.255);
  aCamera->SetViewUp(0, 0, 1);
  aCamera->Azimuth(60);
  aCamera->Elevation(30);
  aCamera->Dolly(1.4);
  aren->ResetCameraClippingRange();

  renWin->SetSize(640, 512);
  renWin->Render();
  renWin->SetWindowName("Kitchen");

  // interact with data
  iren->Start();

  return EXIT_SUCCESS;
}
