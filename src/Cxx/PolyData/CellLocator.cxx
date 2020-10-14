#include <vtkCellLocator.h>
#include <vtkNew.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  // Create the tree
  vtkNew<vtkCellLocator> cellLocator;
  cellLocator->SetDataSet(sphereSource->GetOutput());
  cellLocator->BuildLocator();

  double testPoint[3] = {2.0, 0.0, 0.0};

  // Find the closest points to TestPoint
  double closestPoint[3];   // the coordinates of the closest point will be
                            // returned here
  double closestPointDist2; // the squared distance to the closest point will be
                            // returned here
  vtkIdType cellId; // the cell id of the cell containing the closest point will
                    // be returned here
  int subId;        // this is rarely used (in triangle strips only, I believe)
  cellLocator->FindClosestPoint(testPoint, closestPoint, cellId, subId,
                                closestPointDist2);

  std::cout << "Coordinates of closest point: " << closestPoint[0] << " "
            << closestPoint[1] << " " << closestPoint[2] << std::endl;
  std::cout << "Squared distance to closest point: " << closestPointDist2
            << std::endl;
  std::cout << "CellId: " << cellId << std::endl;

  return EXIT_SUCCESS;
}
