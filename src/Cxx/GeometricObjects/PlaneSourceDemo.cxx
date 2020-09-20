#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkColor.h>
#include <vtkLegendBoxActor.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include <array>

namespace {
vtkSmartPointer<vtkPolyData> CreateArrow(double& length,
                                         std::array<double, 3>& startPoint,
                                         std::array<double, 3>& endPoint);
}

int main(int, char* argv[])
{
  vtkNew<vtkNamedColors> colors;
  vtkColor3d backgroundColor = colors->GetColor3d("DarkSlateGray");
  vtkColor3d legendBackgroundColor = colors->GetColor3d("SlateGray");
  vtkColor3d originColor = colors->GetColor3d("OrangeRed");
  vtkColor3d centerColor = colors->GetColor3d("Gold");
  vtkColor3d point1Color = colors->GetColor3d("MediumSpringGreen");
  vtkColor3d point2Color = colors->GetColor3d("Brown");
  vtkColor3d xAxisColor = colors->GetColor3d("lime");
  vtkColor3d yAxisColor = colors->GetColor3d("orange");
  vtkColor3d normalColor = colors->GetColor3d("Raspberry");

  // Create actors
  vtkNew<vtkPlaneSource> planeSource;
  planeSource->SetOrigin(0.0, 0.0, 0.0);
  planeSource->SetPoint1(1, 0, 0);
  planeSource->SetPoint2(0, 1.0, 0);
  planeSource->Update();

  std::array<double, 6> bounds;
  planeSource->GetOutput()->GetBounds(bounds.data());
  double length = std::max(bounds[1] - bounds[0], bounds[3] - bounds[2]);

  vtkNew<vtkPolyDataMapper> planeMapper;
  planeMapper->SetInputConnection(planeSource->GetOutputPort());
  vtkNew<vtkActor> planeActor;
  planeActor->SetMapper(planeMapper);

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(length * .04);
  vtkNew<vtkPolyDataMapper> originMapper;
  originMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> originActor;
  originActor->SetPosition(planeSource->GetOrigin());
  originActor->SetMapper(originMapper);
  originActor->GetProperty()->SetDiffuseColor(originColor.GetData());

  vtkNew<vtkPolyDataMapper> centerMapper;
  centerMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> centerActor;
  centerActor->SetPosition(planeSource->GetCenter());
  centerActor->SetMapper(centerMapper);
  centerActor->GetProperty()->SetDiffuseColor(centerColor.GetData());

  vtkNew<vtkPolyDataMapper> point1Mapper;
  point1Mapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> point1Actor;
  point1Actor->SetPosition(planeSource->GetPoint1());
  point1Actor->SetMapper(point1Mapper);
  point1Actor->GetProperty()->SetDiffuseColor(point1Color.GetData());

  vtkNew<vtkPolyDataMapper> point2Mapper;
  point2Mapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> point2Actor;
  point2Actor->SetPosition(planeSource->GetPoint2());
  point2Actor->SetMapper(point2Mapper);
  point2Actor->GetProperty()->SetDiffuseColor(point2Color.GetData());

  std::array<double, 3> center;
  std::array<double, 3> origin;
  std::array<double, 3> normal;
  std::array<double, 3> point1;
  std::array<double, 3> point2;
  for (auto i = 0; i < 3; ++i)
  {
    point1[i] = planeSource->GetPoint1()[i];
    point2[i] = planeSource->GetPoint2()[i];
    origin[i] = planeSource->GetOrigin()[i];
    center[i] = planeSource->GetCenter()[i];
    normal[i] = planeSource->GetNormal()[i] * length;
  }
  auto xAxisPolyData = CreateArrow(length, origin, point1);
  vtkNew<vtkPolyDataMapper> xAxisMapper;
  xAxisMapper->SetInputData(xAxisPolyData);
  vtkNew<vtkActor> xAxisActor;
  xAxisActor->SetMapper(xAxisMapper);
  xAxisActor->GetProperty()->SetDiffuseColor(xAxisColor.GetData());

  auto yAxisPolyData = CreateArrow(length, origin, point2);
  vtkNew<vtkPolyDataMapper> yAxisMapper;
  yAxisMapper->SetInputData(yAxisPolyData);
  vtkNew<vtkActor> yAxisActor;
  yAxisActor->SetMapper(yAxisMapper);
  yAxisActor->GetProperty()->SetDiffuseColor(yAxisColor.GetData());

  auto normalPolyData = CreateArrow(length, origin, normal);
  vtkNew<vtkPolyDataMapper> normalMapper;
  normalMapper->SetInputData(normalPolyData);
  vtkNew<vtkActor> normalActor;
  normalActor->SetMapper(normalMapper);
  normalActor->GetProperty()->SetDiffuseColor(normalColor.GetData());

  // Create the RenderWindow, Renderer
  //
  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkLegendBoxActor> legend;
  legend->SetNumberOfEntries(7);
  legend->UseBackgroundOn();
  legend->SetBackgroundColor(legendBackgroundColor.GetData());
  legend->GetPositionCoordinate()->SetValue(.7, .7);
  legend->GetPosition2Coordinate()->SetValue(.3, .3);
  int entry = 0;
  legend->SetEntry(entry++, sphereSource->GetOutput(), "center",
                   centerColor.GetData());
  legend->SetEntry(entry++, sphereSource->GetOutput(), "origin",
                   originColor.GetData());
  legend->SetEntry(entry++, sphereSource->GetOutput(), "point1",
                   point1Color.GetData());
  legend->SetEntry(entry++, sphereSource->GetOutput(), "point2",
                   point2Color.GetData());
  legend->SetEntry(entry++, xAxisPolyData, "xAxis", xAxisColor.GetData());
  legend->SetEntry(entry++, xAxisPolyData, "yAxis", yAxisColor.GetData());
  legend->SetEntry(entry++, xAxisPolyData, "normal", normalColor.GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("PlaneSourceDemo");
  renderWindow->SetSize(640, 480);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->SetBackground(backgroundColor.GetData());
  renderer->AddActor(planeActor);
  renderer->AddActor(originActor);
  renderer->AddActor(centerActor);
  renderer->AddActor(point1Actor);
  renderer->AddActor(point2Actor);
  renderer->AddActor(xAxisActor);
  renderer->AddActor(yAxisActor);
  renderer->AddActor(normalActor);
  renderer->AddActor(legend);

  renderWindow->Render();
  renderer->GetActiveCamera()->SetPosition(1, 0, 0);
  renderer->GetActiveCamera()->SetFocalPoint(0, 1, 0);
  renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCamera();
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkPolyData> CreateArrow(double& pdLength,
                                         std::array<double, 3>& startPoint,
                                         std::array<double, 3>& endPoint)
{
  vtkSmartPointer<vtkPolyData> polyData;

  // Create an arrow.
  vtkNew<vtkArrowSource> arrowSource;
  arrowSource->SetShaftRadius(pdLength * .01);
  arrowSource->SetShaftResolution(20);
  arrowSource->SetTipLength(pdLength * .1);
  arrowSource->SetTipRadius(pdLength * .05);
  arrowSource->SetTipResolution(20);

  // Compute a basis
  std::array<double, 3> normalizedX;
  std::array<double, 3> normalizedY;
  std::array<double, 3> normalizedZ;

  // The X axis is a vector from start to end
  vtkMath::Subtract(endPoint.data(), startPoint.data(), normalizedX.data());
  double length = vtkMath::Norm(normalizedX.data());
  vtkMath::Normalize(normalizedX.data());

  // The Z axis is an arbitrary vector cross X
  vtkNew<vtkMinimalStandardRandomSequence> rng;
  rng->SetSeed(8775070); // For testing.

  std::array<double, 3> arbitrary;
  for (auto i = 0; i < 3; ++i)
  {
    rng->Next();
    arbitrary[i] = rng->GetRangeValue(-10, 10);
  }
  vtkMath::Cross(normalizedX.data(), arbitrary.data(), normalizedZ.data());
  vtkMath::Normalize(normalizedZ.data());

  // The Y axis is Z cross X
  vtkMath::Cross(normalizedZ.data(), normalizedX.data(), normalizedY.data());
  vtkNew<vtkMatrix4x4> matrix;

  // Create the direction cosine matrix
  matrix->Identity();
  for (auto i = 0; i < 3; i++)
  {
    matrix->SetElement(i, 0, normalizedX[i]);
    matrix->SetElement(i, 1, normalizedY[i]);
    matrix->SetElement(i, 2, normalizedZ[i]);
  }

  // Apply the transforms
  vtkNew<vtkTransform> transform;
  transform->Translate(startPoint.data());
  transform->Concatenate(matrix);
  transform->Scale(length, length, length);

  // Transform the polydata
  vtkNew<vtkTransformPolyDataFilter> transformPD;
  transformPD->SetTransform(transform);
  transformPD->SetInputConnection(arrowSource->GetOutputPort());
  transformPD->Update();
  polyData = transformPD->GetOutput();
  return polyData;
}
} // namespace
