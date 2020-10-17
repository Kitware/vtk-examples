#include <vtkCellArray.h>
#include <vtkGeometryFilter.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkVertex.h>

namespace {
template <typename T>
vtkSmartPointer<vtkUnstructuredGrid> MakeUnstructuredGrid(vtkSmartPointer<T>);
}

int main(int argc, char* argv[])
{
  auto geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New();
  if (argc < 2)
  {
    // std::cerr << "Usage: " << argv[0] << " Filename(.vtk) e.g. uGridEx.vtk "
    //          << std::endl;
    auto unstructuredGrid =
        MakeUnstructuredGrid(vtkSmartPointer<vtkVertex>::New());
    geometryFilter->SetInputData(unstructuredGrid);
  }
  else
  {
    // Create the reader for the data.
    // This is the data that will be rendered.
    std::string filename = argv[1];
    std::cout << "Loading " << filename.c_str() << std::endl;
    vtkNew<vtkUnstructuredGridReader> reader;
    reader->SetFileName(filename.c_str());
    geometryFilter->SetInputConnection(reader->GetOutputPort());
  }

  geometryFilter->Update();

  vtkPolyData* polydata = geometryFilter->GetOutput();

  std::cout << "Output has " << polydata->GetNumberOfPoints() << " points."
            << std::endl;

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
} // namespace
