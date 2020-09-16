#include <vtkLine.h>
#include <vtkNew.h>
#include <vtkOBBTree.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  // Create the locator
  vtkNew<vtkOBBTree> tree;
  tree->SetDataSet(sphereSource->GetOutput());
  tree->BuildLocator();

  // Intersect the locator with the line
  double lineP0[3] = {0.0, 0.0, 0.0};
  double lineP1[3] = {0.0, 0.0, 2.0};
  vtkNew<vtkPoints> intersectPoints;

  tree->IntersectWithLine(lineP0, lineP1, intersectPoints, NULL);

  std::cout << "NumPoints: " << intersectPoints->GetNumberOfPoints()
            << std::endl;

  // Display list of intersections
  double intersection[3];
  for (int i = 0; i < intersectPoints->GetNumberOfPoints(); i++)
  {
    intersectPoints->GetPoint(i, intersection);
    std::cout << "Intersection " << i << ": " << intersection[0] << ", "
              << intersection[1] << ", " << intersection[2] << std::endl;
  }

  return EXIT_SUCCESS;
}
