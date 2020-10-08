#include <vtkCellArray.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkTriangle.h>

int main(int, char*[])
{
  // Create a triangle
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(0.0, 1.0, 0.0);

  vtkNew<vtkTriangle> triangle;
  triangle->GetPointIds()->SetId(0, 0);
  triangle->GetPointIds()->SetId(1, 1);
  triangle->GetPointIds()->SetId(2, 2);

  vtkNew<vtkCellArray> triangles;
  triangles->InsertNextCell(triangle);

  // Create a polydata object
  vtkNew<vtkPolyData> polyData;

  // Add the geometry and topology to the polydata
  polyData->SetPoints(points);
  polyData->SetPolys(triangles);

  std::cout << "There are " << polyData->GetNumberOfCells() << " cells."
            << std::endl;

  polyData->GetPolys()->InsertNextCell(triangle);

  std::cout << "There are " << polyData->GetNumberOfCells() << " cells."
            << std::endl;

  return EXIT_SUCCESS;
}
