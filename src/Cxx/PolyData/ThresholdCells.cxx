#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkThreshold.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>

int main(int, char*[])
{
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 1, 1);
  points->InsertNextPoint(2, 2, 2);
  points->InsertNextPoint(3, 3, 3);
  points->InsertNextPoint(4, 4, 4);

  // Create three triangles
  vtkNew<vtkTriangle> triangle0;
  triangle0->GetPointIds()->SetId(0, 0);
  triangle0->GetPointIds()->SetId(1, 1);
  triangle0->GetPointIds()->SetId(2, 2);

  vtkNew<vtkTriangle> triangle1;
  triangle1->GetPointIds()->SetId(0, 1);
  triangle1->GetPointIds()->SetId(1, 2);
  triangle1->GetPointIds()->SetId(2, 3);

  vtkNew<vtkTriangle> triangle2;
  triangle2->GetPointIds()->SetId(0, 2);
  triangle2->GetPointIds()->SetId(1, 3);
  triangle2->GetPointIds()->SetId(2, 4);

  // Add the triangles to a cell array
  vtkNew<vtkCellArray> triangles;
  triangles->InsertNextCell(triangle0);
  triangles->InsertNextCell(triangle1);
  triangles->InsertNextCell(triangle2);

  // Setup index array
  vtkNew<vtkIntArray> index;
  index->SetNumberOfComponents(1);
  index->SetName("index");
  index->InsertNextValue(0);
  index->InsertNextValue(1);
  index->InsertNextValue(2);

  // Add points, cells and index array to a polydata
  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->SetPolys(triangles);
  polydata->GetCellData()->AddArray(index);

  std::cout << "There are " << polydata->GetNumberOfCells()
            << " cells before thresholding." << std::endl;

  vtkNew<vtkThreshold> threshold;
  threshold->SetInputData(polydata);
  threshold->SetLowerThreshold(1);
  threshold->SetThresholdFunction(vtkThreshold::THRESHOLD_LOWER);
  // doesn't work because the array is not added as SCALARS, i.e. via SetScalars
  // threshold->SetInputArrayToProcess(0, 0, 0,
  // vtkDataObject::FIELD_ASSOCIATION_CELLS, vtkDataSetAttributes::SCALARS); use
  // like this:
  threshold->SetInputArrayToProcess(
      0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "index");
  threshold->Update();

  vtkUnstructuredGrid* thresholdedPolydata = threshold->GetOutput();
  std::cout << "There are " << thresholdedPolydata->GetNumberOfCells()
            << " cells after thresholding." << std::endl;

  return EXIT_SUCCESS;
}
