#include <vtkCellArray.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertex.h>

namespace {
template <typename T>
vtkSmartPointer<vtkUnstructuredGrid> MakeUnstructuredGrid(vtkSmartPointer<T>);
}

int main(int, char*[])
{

  auto unstructuredGrid =
      MakeUnstructuredGrid(vtkSmartPointer<vtkVertex>::New());

  vtkNew<vtkDataSetSurfaceFilter> surfaceFilter;
  surfaceFilter->SetInputData(unstructuredGrid);
  surfaceFilter->Update();

  vtkPolyData* polydata = surfaceFilter->GetOutput();

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
