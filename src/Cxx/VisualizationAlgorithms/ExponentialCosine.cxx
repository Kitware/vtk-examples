//
// Brute force computation of Bessel functions. Might be better to create a
// filter (or source) object. Might also consider vtkSampleFunction.

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkWarpScalar.h>

int main(int, char*[])
{

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // create plane to warp
  vtkNew<vtkPlaneSource> plane;
  plane->SetResolution(300, 300);

  vtkNew<vtkTransform> transform;
  transform->Scale(10.0, 10.0, 1.0);

  vtkNew<vtkTransformPolyDataFilter> transF;
  transF->SetInputConnection(plane->GetOutputPort());
  transF->SetTransform(transform);
  transF->Update();

  // compute Bessel function and derivatives. This portion could be
  // encapsulated into source or filter object.
  //
  auto input = transF->GetOutput();
  auto numPts = input->GetNumberOfPoints();

  vtkNew<vtkPoints> newPts;
  newPts->SetNumberOfPoints(numPts);

  vtkNew<vtkDoubleArray> derivs;
  derivs->SetNumberOfTuples(numPts);

  vtkNew<vtkPolyData> bessel;
  bessel->CopyStructure(input);
  bessel->SetPoints(newPts);
  bessel->GetPointData()->SetScalars(derivs);

  double x[3];

  for (auto i = 0; i < numPts; i++)
  {
    input->GetPoint(i, x);
    auto r = sqrt(static_cast<double>(x[0] * x[0]) + x[1] * x[1]);
    x[2] = exp(-r) * cos(10.0 * r);
    newPts->SetPoint(i, x);
    auto deriv = -exp(-r) * (cos(10.0 * r) + 10.0 * sin(10.0 * r));
    derivs->SetValue(i, deriv);
  }

  // warp plane
  vtkNew<vtkWarpScalar> warp;
  warp->SetInputData(bessel);
  warp->XYPlaneOn();
  warp->SetScaleFactor(0.5);

  // mapper and actor
  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(warp->GetOutputPort());
  double tmp[2];
  bessel->GetScalarRange(tmp);
  mapper->SetScalarRange(tmp[0], tmp[1]);

  vtkNew<vtkActor> carpet;
  carpet->SetMapper(mapper);

  // assign our actor to the renderer
  ren->AddActor(carpet);
  ren->SetBackground(colors->GetColor3d("Beige").GetData());
  renWin->SetSize(640, 480);
  renWin->SetWindowName("ExponentialCosine");

  // draw the resulting scene
  ren->ResetCamera();
  ren->GetActiveCamera()->Zoom(1.35);
  ren->GetActiveCamera()->Elevation(-55);
  ren->GetActiveCamera()->Azimuth(25);
  ren->ResetCameraClippingRange();
  renWin->Render();

  iren->Start();

  return EXIT_SUCCESS;
}
