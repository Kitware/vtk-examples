#include <vtkCellArray.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkStructuredGrid.h>
#include <vtkXMLStructuredGridWriter.h>

int main(int, char*[])
{
  // Create a grid
  vtkNew<vtkStructuredGrid> structuredGrid;

  vtkNew<vtkPoints> points;

  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 0, 0);
  points->InsertNextPoint(0, 1, 0);
  points->InsertNextPoint(1, 1, 0);
  points->InsertNextPoint(0, 2, 0);
  points->InsertNextPoint(1, 2, 1);

  // Specify the dimensions of the grid
  structuredGrid->SetDimensions(2, 3, 1);
  structuredGrid->SetPoints(points);

  // Write file
  vtkNew<vtkXMLStructuredGridWriter> writer;
  writer->SetFileName("output.vts");
  writer->SetInputData(structuredGrid);
  writer->Write();

  return EXIT_SUCCESS;
}
