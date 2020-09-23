#include <vtkCellArray.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>

int main(int, char*[])
{
  // Create 8 Vertices.
  vtkNew<vtkPoints> points;

  for (unsigned int i = 0; i < 2; ++i)
    for (unsigned int j = 0; j < 4; ++j) points->InsertNextPoint(i, j, 0);

  // Create Edges
  vtkNew<vtkPolyLine> line;
  line->GetPointIds()->SetNumberOfIds(8);
  for (unsigned int i = 0; i < 8; ++i) line->GetPointIds()->SetId(i, i);

  vtkNew<vtkCellArray> cellArray;
  cellArray->InsertNextCell(line);

  // Create a Graph with Vertices and Edges.
  vtkNew<vtkUnstructuredGrid> grid;
  grid->SetPoints(points);
  grid->SetCells(VTK_POLY_LINE, cellArray);

  // Write the file
  vtkNew<vtkXMLUnstructuredGridWriter> writer;
  writer->SetFileName("vertex.vtu");
  writer->SetInputData(grid);
  writer->Write();

  return EXIT_SUCCESS;
}
