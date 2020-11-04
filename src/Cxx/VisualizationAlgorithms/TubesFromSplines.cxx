#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkTubeFilter.h>
#include <vtkTupleInterpolator.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkPoints> points;
  points->InsertPoint(0, 1, 0, 0);
  points->InsertPoint(1, 2, 0, 0);
  points->InsertPoint(2, 3, 1, 0);
  points->InsertPoint(3, 4, 1, 0);
  points->InsertPoint(4, 5, 0, 0);
  points->InsertPoint(5, 6, 0, 0);

  // Fit a spline to the points
  vtkNew<vtkParametricSpline> spline;
  spline->SetPoints(points);
  vtkNew<vtkParametricFunctionSource> functionSource;
  functionSource->SetParametricFunction(spline);
  functionSource->SetUResolution(10 * points->GetNumberOfPoints());
  functionSource->Update();

  // Interpolate the scalars
  double rad;
  vtkNew<vtkTupleInterpolator> interpolatedRadius;
  interpolatedRadius->SetInterpolationTypeToLinear();
  interpolatedRadius->SetNumberOfComponents(1);
  rad = .2;
  interpolatedRadius->AddTuple(0, &rad);
  rad = .2;
  interpolatedRadius->AddTuple(1, &rad);
  rad = .2;
  interpolatedRadius->AddTuple(2, &rad);
  rad = .1;
  interpolatedRadius->AddTuple(3, &rad);
  rad = .1;
  interpolatedRadius->AddTuple(4, &rad);
  rad = .1;
  interpolatedRadius->AddTuple(5, &rad);

  // Generate the radius scalars
  vtkNew<vtkDoubleArray> tubeRadius;
  unsigned int n = functionSource->GetOutput()->GetNumberOfPoints();
  tubeRadius->SetNumberOfTuples(n);
  tubeRadius->SetName("TubeRadius");
  auto tMin = interpolatedRadius->GetMinimumT();
  auto tMax = interpolatedRadius->GetMaximumT();
  double r;
  for (unsigned int i = 0; i < n; ++i)
  {
    auto t = (tMax - tMin) / (n - 1) * i + tMin;
    interpolatedRadius->InterpolateTuple(t, &r);
    tubeRadius->SetTuple1(i, r);
  }

  // Add the scalars to the polydata
  auto tubePolyData = functionSource->GetOutput();
  tubePolyData->GetPointData()->AddArray(tubeRadius);
  tubePolyData->GetPointData()->SetActiveScalars("TubeRadius");

  // Create the tubes
  vtkNew<vtkTubeFilter> tuber;
  tuber->SetInputData(tubePolyData);
  tuber->SetNumberOfSides(20);
  tuber->SetVaryRadiusToVaryRadiusByAbsoluteScalar();

  //--------------
  // Setup actors and mappers
  vtkNew<vtkPolyDataMapper> lineMapper;
  lineMapper->SetInputData(tubePolyData);
  lineMapper->SetScalarRange(tubePolyData->GetScalarRange());

  vtkNew<vtkPolyDataMapper> tubeMapper;
  tubeMapper->SetInputConnection(tuber->GetOutputPort());
  tubeMapper->SetScalarRange(tubePolyData->GetScalarRange());

  vtkNew<vtkActor> lineActor;
  lineActor->SetMapper(lineMapper);
  lineActor->GetProperty()->SetLineWidth(3);
  vtkNew<vtkActor> tubeActor;
  tubeActor->SetMapper(tubeMapper);
  tubeActor->GetProperty()->SetOpacity(0.6);

  // Setup render window, renderer, and interactor
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkRenderer> renderer;
  renderer->UseHiddenLineRemovalOn();
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("TubesFromSplines");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->AddActor(lineActor);
  renderer->AddActor(tubeActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
