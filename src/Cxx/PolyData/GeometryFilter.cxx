#include <vtkCellArray.h>
#include <vtkGeometryFilter.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

namespace {
template <typename T>
vtkSmartPointer<vtkUnstructuredGrid> MakeUnstructuredGrid(vtkSmartPointer<T>);
}

int main(int, char*[])
{
  vtkNew<vtkUnstructuredGrid> unstructuredGrid;

  vtkNew<vtkGeometryFilter> geometryFilter;
  geometryFilter->SetInputData(unstructuredGrid);
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
