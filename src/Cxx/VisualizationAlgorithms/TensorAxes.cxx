// Translated from tenAxes.tcl

#include <vtkAxes.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPointLoad.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3D.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTensorGlyph.h>
#include <vtkTubeFilter.h>

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

  // Generate the tensor axes.
  vtkNew<vtkAxes> axes;
  axes->SetScaleFactor(0.5);

  vtkNew<vtkTubeFilter> tubeAxes;
  tubeAxes->SetInputConnection(axes->GetOutputPort());
  tubeAxes->SetRadius(0.1);
  tubeAxes->SetNumberOfSides(6);

  vtkNew<vtkTensorGlyph> tensorAxes;
  tensorAxes->SetInputConnection(ptLoad->GetOutputPort());
  tensorAxes->SetSourceConnection(axes->GetOutputPort());
  tensorAxes->SetScaleFactor(10);
  tensorAxes->ClampScalingOn();

  // Map contour
  vtkNew<vtkLookupTable> lut;
  MakeLogLUT(lut);

  vtkNew<vtkPolyDataMapper> tensorAxesMapper;
  tensorAxesMapper->SetInputConnection(tensorAxes->GetOutputPort());
  tensorAxesMapper->SetLookupTable(lut);
  plane->Update(); // force update for scalar range

  tensorAxesMapper->SetScalarRange(plane->GetOutput()->GetScalarRange());

  vtkNew<vtkActor> tensorActor;
  tensorActor->SetMapper(tensorAxesMapper);

  // Create an outline around the data.
  //
  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(ptLoad->GetOutputPort());

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  //
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
  coneActor->GetProperty()->SetColor(colors->GetColor3d("BurlyWood").GetData());

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
  renWin->SetWindowName("TensorAxes");

  iren->Initialize();
  renWin->Render();
  iren->Start();
  return EXIT_SUCCESS;
}

namespace {
void MakeLogLUT(vtkLookupTable* lut)
{
  // Original
  lut->SetScaleToLog10();
  lut->SetHueRange(.6667, 0.0);
  lut->Build();
}
} // namespace
