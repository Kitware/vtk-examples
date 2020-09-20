#include <vtkNew.h>
#include <vtkPlanesIntersection.h>
#include <vtkPoints.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  double bounds[6];
  sphereSource->GetOutput()->GetBounds(bounds);

  vtkNew<vtkPoints> box;

  box->SetNumberOfPoints(8);

  double xMin, xMax, yMin, yMax, zMin, zMax;
  xMin = bounds[0];
  xMax = bounds[1];
  yMin = bounds[2];
  yMax = bounds[3];
  zMin = bounds[4];
  zMax = bounds[5];

  box->SetPoint(0, xMax, yMin, zMax);
  box->SetPoint(1, xMax, yMin, zMin);
  box->SetPoint(2, xMax, yMax, zMin);
  box->SetPoint(3, xMax, yMax, zMax);
  box->SetPoint(4, xMin, yMin, zMax);
  box->SetPoint(5, xMin, yMin, zMin);
  box->SetPoint(6, xMin, yMax, zMin);
  box->SetPoint(7, xMin, yMax, zMax);

  vtkNew<vtkPlanesIntersection> planesIntersection;
  planesIntersection->SetBounds(bounds);

  int intersects = planesIntersection->IntersectsRegion(box);
  auto res = (intersects == 1) ? "Yes" : "No";

  std::cout << "Intersects? " << res << std::endl;

  return EXIT_SUCCESS;
}
