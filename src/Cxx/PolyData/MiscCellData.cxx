#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkTriangle.h>
#include <vtkXMLPolyDataWriter.h>

int main(int, char*[])
{
  // We will write the resulting file to Test.vtp so it can be inspected in
  // Paraview
  std::string outputFilename = "output.vtp";

  // Setup 3 points
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(0.0, 1.0, 0.0);

  // Create a triangle
  vtkNew<vtkTriangle> triangle;
  triangle->GetPointIds()->SetId(0, 0);
  triangle->GetPointIds()->SetId(1, 1);
  triangle->GetPointIds()->SetId(2, 2);

  // Add the triangle to a cell array
  vtkNew<vtkCellArray> triangles;
  triangles->InsertNextCell(triangle);

  // Setup data for the triangle. Attach a value of 1.45.
  // This can be anything you wish to store with it)
  vtkNew<vtkDoubleArray> triangleData;
  triangleData->SetNumberOfComponents(
      1); // we will have only 1 value associated with the triangle
  triangleData->SetName("TriangleData"); // set the name of the value
  triangleData->InsertNextValue(1.45);   // set the actual value

  // Create a polydata that contains the points,
  // the triangle on those points, and the data
  // array (value) we created for the triangle
  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->SetPolys(triangles);
  polydata->GetCellData()->AddArray(triangleData);

  // Write the file
  vtkNew<vtkXMLPolyDataWriter> writer;
  writer->SetInputData(polydata);
  writer->SetFileName(outputFilename.c_str());
  writer->Write();

  return EXIT_SUCCESS;
}
