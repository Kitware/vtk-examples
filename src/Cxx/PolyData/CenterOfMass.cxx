#include <vtkCenterOfMass.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <cmath>
#include <limits>

int main(int, char*[])
{
  // Create a point set of a square
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 0, 0);
  points->InsertNextPoint(0, 1, 0);
  points->InsertNextPoint(1, 1, 0);

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);

  // Compute the center of mass
  vtkNew<vtkCenterOfMass> centerOfMassFilter;
  centerOfMassFilter->SetInputData(polydata);
  centerOfMassFilter->SetUseScalarsAsWeights(false);
  centerOfMassFilter->Update();

  double center[3];
  centerOfMassFilter->GetCenter(center);

  std::cout << "Center of mass is " << center[0] << " " << center[1] << " "
            << center[2] << std::endl;

  return EXIT_SUCCESS;
}
