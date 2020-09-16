#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkKdTreePointLocator.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

int main(int, char*[])
{
  // Create some random points
  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(10);
  pointSource->Update();

  // Create the tree
  vtkNew<vtkKdTreePointLocator> pointTree;
  pointTree->SetDataSet(pointSource->GetOutput());
  pointTree->BuildLocator();

  // Find the k closest points to (0,0,0)
  vtkIdType k = 1;
  double testPoint[3] = {0.0, 0.0, 0.0};
  vtkNew<vtkIdList> result;

  pointTree->FindPointsWithinRadius(1.0, testPoint, result);

  for (vtkIdType i = 0; i < k; i++)
  {
    vtkIdType point_ind = result->GetId(i);
    double p[3];
    pointSource->GetOutput()->GetPoint(point_ind, p);
    std::cout << "Closest point " << i << ": Point " << point_ind << ": ("
              << p[0] << ", " << p[1] << ", " << p[2] << ")" << std::endl;
  }

  return EXIT_SUCCESS;
}
