#include <vtkIdList.h>
#include <vtkKdTree.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>

int main(int, char*[])
{
  // Create some points
  double origin[3] = {0.0, 0.0, 0.0};
  double x[3] = {1.0, 0.0, 0.0};
  double y[3] = {0.0, 1.0, 0.0};
  double z[3] = {0.0, 0.0, 1.0};

  vtkNew<vtkPoints> points;
  points->InsertNextPoint(origin);
  points->InsertNextPoint(x);
  points->InsertNextPoint(y);
  points->InsertNextPoint(z);

  // Create the tree
  vtkNew<vtkKdTree> pointTree;
  pointTree->BuildLocatorFromPoints(points);

  // Find the 2 closest points to (0.5,0,0)
  vtkIdType k = 2;
  double testPoint[3] = {0.5, 0.0, 0.0};
  vtkNew<vtkIdList> result;

  pointTree->FindClosestNPoints(k, testPoint, result);

  for (vtkIdType i = 0; i < k; i++)
  {
    vtkIdType point_ind = result->GetId(i);
    double p[3];
    points->GetPoint(point_ind, p);
    std::cout << "Closest point " << i << ": Point " << point_ind << ": ("
              << p[0] << ", " << p[1] << ", " << p[2] << ")" << std::endl;
  }

  return EXIT_SUCCESS;
}
