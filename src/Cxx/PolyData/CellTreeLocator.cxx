#include <vtkCellTreeLocator.h>
#include <vtkGenericCell.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>

// Note that:
// vtkCellTreeLocator moved from vtkFiltersGeneral to vtkCommonDataModel in
// VTK commit 4a29e6f7dd9acb460644fe487d2e80aac65f7be9

int main(int, char*[])
{
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(1.0);
  sphereSource->Update();

  // Create the tree
  vtkNew<vtkCellTreeLocator> cellTree;
  cellTree->SetDataSet(sphereSource->GetOutput());
  cellTree->BuildLocator();

  double testInside[3] = {0.5, 0.0, 0.0};
  double testOutside[3] = {10.0, 0.0, 0.0};

  double pcoords[3], weights[3];

  vtkIdType cellId;

  vtkNew<vtkGenericCell> cell;

  int returnValue = EXIT_SUCCESS;

  // Should be inside
  cellId = cellTree->FindCell(testInside, 0, cell, pcoords, weights);
  if (cellId >= 0)
  {
    std::cout << "First point: in cell " << cellId << std::endl;
  }
  else
  {
    std::cout << "ERROR: Cell was not found but should have been." << std::endl;
    returnValue = EXIT_FAILURE;
  }

  // Should be outside
  cellId = cellTree->FindCell(testOutside, 0, cell, pcoords, weights);
  if (cellId >= 0)
  {
    std::cout << "ERROR: Found point in cell " << cellId
              << " but it should be outside the domain." << std::endl;
    returnValue = EXIT_FAILURE;
  }
  else
  {
    std::cout << "Second point: outside" << std::endl;
  }

  return returnValue;
}
