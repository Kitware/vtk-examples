#include <vtkCellArray.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridWriter.h>

#include <vtkHexagonalPrism.h>
#include <vtkHexahedron.h>
#include <vtkLine.h>
#include <vtkPentagonalPrism.h>
#include <vtkPixel.h>
#include <vtkPolyLine.h>
#include <vtkPolyVertex.h>
#include <vtkPolygon.h>
#include <vtkPyramid.h>
#include <vtkQuad.h>
#include <vtkTetra.h>
#include <vtkTriangle.h>
#include <vtkTriangleStrip.h>
#include <vtkVertex.h>
#include <vtkVoxel.h>
#include <vtkWedge.h>

#include <cstdlib>
#include <string>
#include <vector>

// These functions return a vtkUnstructured grid corresponding to the object.
namespace {
template <typename T>
vtkSmartPointer<vtkUnstructuredGrid> MakeUnstructuredGrid(vtkSmartPointer<T>);

vtkSmartPointer<vtkUnstructuredGrid> MakePolyVertex();
vtkSmartPointer<vtkUnstructuredGrid> MakePolyLine();
vtkSmartPointer<vtkUnstructuredGrid> MakeTriangleStrip();
vtkSmartPointer<vtkUnstructuredGrid> MakePolygon();
} // namespace

int main(int, char*[])
{
  std::vector<std::string> filenames;
  std::vector<vtkSmartPointer<vtkUnstructuredGrid>> uGrids;

  uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkVertex>::New()));
  filenames.push_back("Vertex.vtk");

  uGrids.push_back(MakePolyVertex());
  filenames.push_back("PolyVertex.vtk");

  uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkLine>::New()));
  filenames.push_back("Line.vtk");

  uGrids.push_back(MakePolyLine());
  filenames.push_back("PolyLine.vtk");

  uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkTriangle>::New()));
  filenames.push_back("Triangle.vtk");

  uGrids.push_back(MakeTriangleStrip());
  filenames.push_back("TriangleStrip.vtk");

  uGrids.push_back(MakePolygon());
  filenames.push_back("Polygon.vtk");

  uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkPixel>::New()));
  filenames.push_back("Pixel.vtk");

  uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkQuad>::New()));
  filenames.push_back("Quad.vtk");

  uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkTetra>::New()));
  filenames.push_back("Tetra.vtk");

  uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkVoxel>::New()));
  filenames.push_back("Voxel.vtk");

  uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkHexahedron>::New()));
  filenames.push_back("Hexahedron.vtk");

  uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkWedge>::New()));
  filenames.push_back("Wedge.vtk");

  uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkPyramid>::New()));
  filenames.push_back("Pyramid.vtk");

  uGrids.push_back(
      MakeUnstructuredGrid(vtkSmartPointer<vtkPentagonalPrism>::New()));
  filenames.push_back("PentagonalPrism.vtk");

  uGrids.push_back(
      MakeUnstructuredGrid(vtkSmartPointer<vtkHexagonalPrism>::New()));
  filenames.push_back("HexagonalPrism.vtk");

  // Write each grid into  a file
  for (unsigned int i = 0; i < uGrids.size(); ++i)
  {
    std::cout << "Writing: " << filenames[i] << std::endl;
    vtkNew<vtkUnstructuredGridWriter> writer;
    writer->SetFileName(filenames[i].c_str());
    writer->SetInputData(uGrids[i]);
    writer->Write();
  }

  return EXIT_SUCCESS;
}
namespace {
template <typename T>
vtkSmartPointer<vtkUnstructuredGrid>
MakeUnstructuredGrid(vtkSmartPointer<T> aCell)
{
  double* pcoords = aCell->GetParametricCoords();
  for (int i = 0; i < aCell->GetNumberOfPoints(); ++i)
  {
    aCell->GetPointIds()->SetId(i, i);
    aCell->GetPoints()->SetPoint(i, *(pcoords + 3 * i), *(pcoords + 3 * i + 1),
                                 *(pcoords + 3 * i + 2));
  }

  vtkNew<vtkUnstructuredGrid> ug;
  ug->SetPoints(aCell->GetPoints());
  ug->InsertNextCell(aCell->GetCellType(), aCell->GetPointIds());
  return ug;
}

vtkSmartPointer<vtkUnstructuredGrid> MakePolyVertex()
{
  // A polyvertex is a cell represents a set of 0D vertices
  int numberOfVertices = 6;

  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 0, 0);
  points->InsertNextPoint(0, 1, 0);
  points->InsertNextPoint(0, 0, 1);
  points->InsertNextPoint(1, 0, .4);
  points->InsertNextPoint(0, 1, .6);

  vtkNew<vtkPolyVertex> polyVertex;
  polyVertex->GetPointIds()->SetNumberOfIds(numberOfVertices);

  for (int i = 0; i < numberOfVertices; ++i)
  {
    polyVertex->GetPointIds()->SetId(i, i);
  }

  vtkNew<vtkUnstructuredGrid> ug;
  ug->SetPoints(points);
  ug->InsertNextCell(polyVertex->GetCellType(), polyVertex->GetPointIds());

  return ug;
}

vtkSmartPointer<vtkUnstructuredGrid> MakePolyLine()
{
  // A polyline is a cell that represents a set of 1D lines
  int numberOfVertices = 5;

  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, .5, 0);
  points->InsertNextPoint(.5, 0, 0);
  points->InsertNextPoint(1, .3, 0);
  points->InsertNextPoint(1.5, .4, 0);
  points->InsertNextPoint(2.0, .4, 0);

  vtkNew<vtkPolyLine> polyline;
  polyline->GetPointIds()->SetNumberOfIds(numberOfVertices);

  for (int i = 0; i < numberOfVertices; ++i)
  {
    polyline->GetPointIds()->SetId(i, i);
  }

  vtkNew<vtkUnstructuredGrid> ug;
  ug->SetPoints(points);
  ug->InsertNextCell(polyline->GetCellType(), polyline->GetPointIds());

  return ug;
}

vtkSmartPointer<vtkUnstructuredGrid> MakeTriangleStrip()
{
  // A triangle is a cell that represents a triangle strip
  int numberOfVertices = 10;

  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(.5, 1, 0);
  points->InsertNextPoint(1, -.1, 0);
  points->InsertNextPoint(1.5, .8, 0);
  points->InsertNextPoint(2.0, -.1, 0);
  points->InsertNextPoint(2.5, .9, 0);
  points->InsertNextPoint(3.0, 0, 0);
  points->InsertNextPoint(3.5, .8, 0);
  points->InsertNextPoint(4.0, -.2, 0);
  points->InsertNextPoint(4.5, 1.1, 0);

  vtkNew<vtkTriangleStrip> trianglestrip;
  trianglestrip->GetPointIds()->SetNumberOfIds(numberOfVertices);
  for (int i = 0; i < numberOfVertices; ++i)
  {
    trianglestrip->GetPointIds()->SetId(i, i);
  }

  vtkNew<vtkUnstructuredGrid> ug;
  ug->SetPoints(points);
  ug->InsertNextCell(trianglestrip->GetCellType(),
                     trianglestrip->GetPointIds());

  return ug;
}

vtkSmartPointer<vtkUnstructuredGrid> MakePolygon()
{
  // A polygon is a cell that represents a polygon
  int numberOfVertices = 6;

  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, -.1, 0);
  points->InsertNextPoint(.8, .5, 0);
  points->InsertNextPoint(1, 1, 0);
  points->InsertNextPoint(.6, 1.2, 0);
  points->InsertNextPoint(0, .8, 0);

  vtkNew<vtkPolygon> polygon;
  polygon->GetPointIds()->SetNumberOfIds(numberOfVertices);
  for (int i = 0; i < numberOfVertices; ++i)
  {
    polygon->GetPointIds()->SetId(i, i);
  }

  vtkNew<vtkUnstructuredGrid> ug;
  ug->SetPoints(points);
  ug->InsertNextCell(polygon->GetCellType(), polygon->GetPointIds());

  return ug;
}
} // namespace
