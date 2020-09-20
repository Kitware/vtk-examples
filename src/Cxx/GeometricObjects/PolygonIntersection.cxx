#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolygon.h>

int main(int, char*[])
{
  // Create a square in the XY plane
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(1.0, 1.0, 0.0);
  points->InsertNextPoint(0.0, 1.0, 0.0);

  // Create the polygon
  vtkNew<vtkPolygon> polygon;
  polygon->GetPoints()->DeepCopy(points);
  polygon->GetPointIds()->SetNumberOfIds(4); // 4 corners of the square
  polygon->GetPointIds()->SetId(0, 0);
  polygon->GetPointIds()->SetId(1, 1);
  polygon->GetPointIds()->SetId(2, 2);
  polygon->GetPointIds()->SetId(3, 3);

  // Inputs
  double p1[3] = {0.1, 0, -1.0};
  double p2[3] = {0.1, 0, 1.0};
  double tolerance = 0.001;
  // Outputs
  double t; // Parametric coordinate of intersection (0 (corresponding to p1) to
            // 1 (corresponding to p2))
  double x[3]; // The coordinate of the intersection
  double pcoords[3];
  int subId;

  vtkIdType iD =
      polygon->IntersectWithLine(p1, p2, tolerance, t, x, pcoords, subId);

  auto res = (iD == 1) ? "Yes" : "No";
  std::cout << "intersected? " << res << std::endl;
  std::cout << "intersection: " << x[0] << " " << x[1] << " " << x[2]
            << std::endl;

  return EXIT_SUCCESS;
}
