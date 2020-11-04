// Translated from tenEllip.tcl

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkColorSeries.h>
#include <vtkConeSource.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPointLoad.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProp3D.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTensorGlyph.h>

namespace {
void MakeLogLUT(vtkLookupTable* lut);
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create the RenderWindow, Renderer and interactive renderer.
  //
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Generate the tensors.
  vtkNew<vtkPointLoad> ptLoad;
  ptLoad->SetLoadValue(100.0);
  ptLoad->SetSampleDimensions(6, 6, 6);
  ptLoad->ComputeEffectiveStressOn();
  ptLoad->SetModelBounds(-10, 10, -10, 10, -10, 10);

  // Extract a plane of data.
  vtkNew<vtkImageDataGeometryFilter> plane;
  plane->SetInputConnection(ptLoad->GetOutputPort());
  plane->SetExtent(2, 2, 0, 99, 0, 99);

  // Generate the ellipsoids.
  vtkNew<vtkSphereSource> sphere;
  sphere->SetThetaResolution(8);
  sphere->SetPhiResolution(8);
  vtkNew<vtkTensorGlyph> tensorEllipsoids;
  tensorEllipsoids->SetInputConnection(ptLoad->GetOutputPort());
  tensorEllipsoids->SetSourceConnection(sphere->GetOutputPort());
  tensorEllipsoids->SetScaleFactor(10);
  tensorEllipsoids->ClampScalingOn();

  vtkNew<vtkPolyDataNormals> ellipNormals;
  ellipNormals->SetInputConnection(tensorEllipsoids->GetOutputPort());

  // Map contour.
  vtkNew<vtkLookupTable> lut;
  MakeLogLUT(lut);
  vtkNew<vtkPolyDataMapper> tensorEllipsoidsMapper;
  tensorEllipsoidsMapper->SetInputConnection(ellipNormals->GetOutputPort());
  tensorEllipsoidsMapper->SetLookupTable(lut);
  plane->Update(); // force update for scalar range
  tensorEllipsoidsMapper->SetScalarRange(plane->GetOutput()->GetScalarRange());

  vtkNew<vtkActor> tensorActor;
  tensorActor->SetMapper(tensorEllipsoidsMapper);

  // Create an outline around the data.
  //
  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(ptLoad->GetOutputPort());

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  // Create a cone whose apex indicates the application of load.
  //
  vtkNew<vtkConeSource> coneSrc;
  coneSrc->SetRadius(0.5);
  coneSrc->SetHeight(2);
  vtkNew<vtkPolyDataMapper> coneMap;
  coneMap->SetInputConnection(coneSrc->GetOutputPort());
  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMap);
  coneActor->SetPosition(0, 0, 11);
  coneActor->RotateY(90);
  coneActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

  vtkNew<vtkCamera> camera;
  camera->SetFocalPoint(0.113766, -1.13665, -1.01919);
  camera->SetPosition(-29.4886, -63.1488, 26.5807);
  camera->SetViewAngle(24.4617);
  camera->SetViewUp(0.17138, 0.331163, 0.927879);
  camera->SetClippingRange(1, 100);

  ren->AddActor(tensorActor);
  ren->AddActor(outlineActor);
  ren->AddActor(coneActor);
  ren->SetBackground(colors->GetColor3d("WhiteSmoke").GetData());
  ren->SetActiveCamera(camera);

  renWin->SetSize(512, 512);
  renWin->SetWindowName("TensorEllipsoids");

  iren->Initialize();
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {
void MakeLogLUT(vtkLookupTable* lut)
{
  // Make the lookup using a Brewer palette.
  vtkNew<vtkColorSeries> colorSeries;
  colorSeries->SetNumberOfColors(8);
  int colorSeriesEnum = colorSeries->BREWER_DIVERGING_SPECTRAL_8;
  colorSeries->SetColorScheme(colorSeriesEnum);
  lut->SetScaleToLog10();
  colorSeries->BuildLookupTable(lut, colorSeries->ORDINAL);
  lut->SetNanColor(1, 0, 0, 1);
  // Original
  // lut->SetScaleToLog10();
  // lut->SetHueRange(.6667, 0.0);
  // lut->Build();
}
} // namespace
