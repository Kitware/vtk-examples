#include <vtkCellArray.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>

#include <iostream>

namespace {
vtkSmartPointer<vtkPolyData> custom_reader(std::ifstream& infile);
}

int main(int argc, char* argv[])
{
  // Verify command line arguments
  if (argc != 2)
  {
    std::cerr << "Required arguments: triangleFile.txt" << std::endl;
    return EXIT_FAILURE;
  }

  std::string inputFilename = argv[1];

  std::ifstream fin(inputFilename.c_str());

  auto polydata = custom_reader(fin);

  vtkNew<vtkXMLPolyDataWriter> writer;
  writer->SetInputData(polydata);
  writer->SetFileName("x.vtp");
  writer->Write();
  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkPolyData> custom_reader(std::ifstream& infile)
{
  vtkIdType number_of_points, number_of_triangles;
  infile >> number_of_points >> number_of_triangles;

  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(number_of_points);
  for (vtkIdType i = 0; i < number_of_points; i++)
  {
    double x, y, z;
    infile >> x >> y >> z;
    points->SetPoint(i, x, y, z);
  }

  vtkNew<vtkCellArray> polys;
  for (vtkIdType i = 0; i < number_of_triangles; i++)
  {
    vtkIdType a, b, c;
    infile >> a >> b >> c;
    polys->InsertNextCell(3);
    polys->InsertCellPoint(a);
    polys->InsertCellPoint(b);
    polys->InsertCellPoint(c);
  }
  auto polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(points);
  polydata->SetPolys(polys);
  return polydata;
}
} // namespace
