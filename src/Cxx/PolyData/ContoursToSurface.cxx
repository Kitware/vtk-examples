#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCellArray.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVoxelContoursToSurfaceFilter.h>

namespace {
void CreateCircle(const double& z, const double& radius, const int& resolution,
                  vtkPolyData* polyData);
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create the data: a series of discs representing the intersections of x-y
  // planes through a unit sphere centered at 0, 0, 0
  //
  int numDivisions = 20;
  int resolution = 100;
  double lastz = 0.0;
  double z = 0.0;
  // double radius = 0.0;
  double sphereRadius = 1.0;
  double zmin = -0.9 * sphereRadius;
  double zmax = 0.9 * sphereRadius;

  // Append all the discs into one polydata
  //
  vtkNew<vtkAppendPolyData> appendFilter;

  for (int i = 0; i <= numDivisions; ++i)
  {
    lastz = z;
    double u = i / double(numDivisions);
    z = (1. - u) * zmin + u * zmax;
    auto radius = sqrt(sphereRadius * sphereRadius - z * z);
    vtkNew<vtkPolyData> circle;
    CreateCircle(z, radius, resolution, circle);
    appendFilter->AddInputData(circle);
  }

  double deltaz = z - lastz;

  if (!appendFilter->GetNumberOfInputConnections(0))
  {
    cerr << "error, no contours!" << endl;
    return EXIT_FAILURE;
  }

  appendFilter->Update();

  // Convert to ijk coordinates for the contour to surface filter
  //
  double bounds[6];
  vtkPolyData* contours = appendFilter->GetOutput();
  contours->GetBounds(bounds);
  double origin[3] = {bounds[0], bounds[2], bounds[4]};
  double spacing[3] = {(bounds[1] - bounds[0]) / 40,
                       (bounds[3] - bounds[2]) / 40, deltaz};

  vtkNew<vtkPolyData> poly;
  vtkNew<vtkPoints> points;
  vtkPoints* contourPoints = contours->GetPoints();
  int numPoints = contourPoints->GetNumberOfPoints();
  points->SetNumberOfPoints(numPoints);
  for (int i = 0; i < numPoints; ++i)
  {
    double pt[3];
    contourPoints->GetPoint(i, pt);
    pt[0] = static_cast<int>((pt[0] - origin[0]) / spacing[0] + 0.5);
    pt[1] = static_cast<int>((pt[1] - origin[1]) / spacing[1] + 0.5);
    pt[2] = static_cast<int>((pt[2] - origin[2]) / spacing[2] + 0.5);
    points->SetPoint(i, pt);
  }
  poly->SetPolys(contours->GetPolys());
  poly->SetPoints(points);

  // Create the contour to surface filter
  //
  vtkNew<vtkVoxelContoursToSurfaceFilter> contoursToSurface;
  contoursToSurface->SetInputData(poly);
  contoursToSurface->SetSpacing(spacing[0], spacing[1], spacing[2]);
  contoursToSurface->Update();

  // Rescale the output back into world coordinates and center it
  //
  double scaleCenter[3];
  contoursToSurface->GetOutput()->GetCenter(scaleCenter);
  double scaleBounds[6];
  contoursToSurface->GetOutput()->GetBounds(scaleBounds);
  double center[3];
  contours->GetCenter(center);

  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(contoursToSurface->GetOutputPort());
  vtkNew<vtkTransform> transform;
  transformFilter->SetTransform(transform);
  transform->Translate(-scaleCenter[0], -scaleCenter[1], -scaleCenter[2]);
  transform->Scale((bounds[1] - bounds[0]) / (scaleBounds[1] - scaleBounds[0]),
                   (bounds[3] - bounds[2]) / (scaleBounds[3] - scaleBounds[2]),
                   (bounds[5] - bounds[4]) / (scaleBounds[5] - scaleBounds[4]));
  transform->Translate(center[0], center[1], center[2]);

  // Visualize the contours
  //
  vtkNew<vtkPolyDataMapper> contoursMapper;
  contoursMapper->SetInputData(contours);
  contoursMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> contoursActor;
  contoursActor->SetMapper(contoursMapper);
  contoursActor->GetProperty()->SetRepresentationToWireframe();
  contoursActor->GetProperty()->ShadingOff();
  contoursActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // Visualize the surface
  //
  vtkNew<vtkPolyDataMapper> surfaceMapper;
  surfaceMapper->SetInputConnection(transformFilter->GetOutputPort());
  surfaceMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> surfaceActor;
  surfaceActor->SetMapper(surfaceMapper);
  surfaceActor->GetProperty()->SetRepresentationToWireframe();
  surfaceActor->GetProperty()->ShadingOff();
  surfaceActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // Create two renderers side by side to show the contours and the surface
  // separately
  //
  std::cout << "Press 't' for trackball interaction" << std::endl;
  std::cout << "Press 'r' to reset the camera" << std::endl;
  std::cout << "Press 'w' for wireframe representation" << std::endl;
  std::cout << "Press 's' for surface representation" << std::endl;

  vtkNew<vtkRenderer> renderer1;
  renderer1->SetViewport(0., 0., 0.5, 1.);
  renderer1->SetBackground(colors->GetColor3d("CadetBlue").GetData());

  vtkNew<vtkRenderer> renderer2;
  renderer2->SetViewport(0.5, 0., 1., 1.);
  renderer2->SetBackground(colors->GetColor3d("BurlyWood").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(800, 400);
  renderWindow->SetWindowName("ContoursToSurface");

  renderWindow->AddRenderer(renderer1);
  renderWindow->AddRenderer(renderer2);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer1->AddViewProp(surfaceActor);
  renderer2->AddViewProp(contoursActor);
  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
void CreateCircle(const double& z, const double& radius, const int& resolution,
                  vtkPolyData* polyData)
{
  vtkNew<vtkPoints> points;
  vtkNew<vtkCellArray> cells;

  points->SetNumberOfPoints(resolution);
  cells->Allocate(1, resolution);
  cells->InsertNextCell(resolution);

  for (int i = 0; i < resolution; ++i)
  {
    double theta = vtkMath::RadiansFromDegrees(360. * i / double(resolution));
    double x = radius * cos(theta);
    double y = radius * sin(theta);
    points->SetPoint(i, x, y, z);
    cells->InsertCellPoint(i);
  }

  polyData->Initialize();
  polyData->SetPolys(cells);
  polyData->SetPoints(points);
}
} // namespace
