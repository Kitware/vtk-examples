#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

int main(int, char*[])
{
  ///////// Set Point Normals ///////////
  // Create 3 points
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(0.0, 1.0, 0.0);

  // Add the points to a polydata
  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);

  // Set point normals
  vtkNew<vtkDoubleArray> pointNormalsArray;
  pointNormalsArray->SetNumberOfComponents(3); // 3d normals (ie x,y,z)
  pointNormalsArray->SetNumberOfTuples(polydata->GetNumberOfPoints());

  // Construct the normal vectors
  double pN1[3] = {1.0, 0.0, 0.0};
  double pN2[3] = {0.0, 1.0, 0.0};
  double pN3[3] = {0.0, 0.0, 1.0};

  // Add the data to the normals array
  pointNormalsArray->SetTuple(0, pN1);
  pointNormalsArray->SetTuple(1, pN2);
  pointNormalsArray->SetTuple(2, pN3);

  // Add the normals to the points in the polydata
  polydata->GetPointData()->SetNormals(pointNormalsArray);

  ///////// Get Point Normals ///////////
  auto pointNormalsRetrieved =
      dynamic_cast<vtkDoubleArray*>(polydata->GetPointData()->GetNormals());
  if (pointNormalsRetrieved)
  {
    std::cout << "There are " << pointNormalsRetrieved->GetNumberOfTuples()
              << " point normals." << std::endl;

    for (vtkIdType i = 0; i < pointNormalsRetrieved->GetNumberOfTuples(); i++)
    {
      double pN[3];
      pointNormalsRetrieved->GetTuple(i, pN);
      std::cout << "Point normal " << i << ": " << pN[0] << " " << pN[1] << " "
                << pN[2] << std::endl;
    }

  } // end if(pointNormalsRetrieved)
  else
  {
    std::cout << "No point normals." << std::endl;
  }

  return EXIT_SUCCESS;
}
