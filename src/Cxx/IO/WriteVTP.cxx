#include <vtkCellArray.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>

int main(int, char*[])
{
  // Create 10 points.
  vtkNew<vtkPoints> points;

  for (unsigned int i = 0; i < 10; ++i)
  {
    points->InsertNextPoint(i, i, i);
  }

  // Create a polydata object and add the points to it.
  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);

  // Write the file
  vtkNew<vtkXMLPolyDataWriter> writer;
  writer->SetFileName("test.vtp");
  writer->SetInputData(polydata);

  // Optional - set the mode. The default is binary.
  // writer->SetDataModeToBinary();
  // writer->SetDataModeToAscii();

  writer->Write();

  return EXIT_SUCCESS;
}
