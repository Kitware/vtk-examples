#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkElevationFilter.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkSphereSource> sphere;
  sphere->SetThetaResolution(12);
  sphere->SetPhiResolution(12);

  vtkNew<vtkTransform> aTransform;
  aTransform->Scale(1, 1.5, 2);

  vtkNew<vtkTransformFilter> transFilter;
  transFilter->SetInputConnection(sphere->GetOutputPort());
  transFilter->SetTransform(aTransform);

  vtkNew<vtkElevationFilter> colorIt;
  colorIt->SetInputConnection(transFilter->GetOutputPort());
  colorIt->SetLowPoint(0, 0, -1);
  colorIt->SetHighPoint(0, 0, 1);

  vtkNew<vtkLookupTable> lut;
  lut->SetHueRange(0.667, 0);
  lut->SetSaturationRange(1, 1);
  lut->SetValueRange(1, 1);

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetLookupTable(lut);
  mapper->SetInputConnection(colorIt->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Elevation(60.0);
  renderer->GetActiveCamera()->Azimuth(30.0);
  renderer->ResetCameraClippingRange();

  renWin->SetSize(640, 480);
  renWin->SetWindowName("TransformSphere");

  renWin->Render();

  // interact with data
  iren->Start();

  return EXIT_SUCCESS;
}
