#include <vtkFloatArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkVectorNorm.h>

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

int main(int, char*[])
{
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(1, 2, 3);
  points->InsertNextPoint(4, 5, 6);

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);

  vtkNew<vtkFloatArray> distances;
  distances->SetNumberOfComponents(3);
  distances->SetName("Distances");

  float v1[3] = {1, 2, 3};
  float v2[3] = {4, 5, 6};
  distances->InsertNextTupleValue(v1);
  distances->InsertNextTupleValue(v2);

  polydata->GetPointData()->SetVectors(distances);

  vtkNew<vtkVectorNorm> vectorNorm;
  vectorNorm->SetInputData(polydata);
  vectorNorm->Update();

  vtkFloatArray* scalars = dynamic_cast<vtkFloatArray*>(
      vectorNorm->GetOutput()->GetPointData()->GetScalars());

  for (vtkIdType i = 0; i < scalars->GetNumberOfTuples(); i++)
  {
    std::cout << "Value " << i << " : " << scalars->GetValue(i) << std::endl;
  }

  return EXIT_SUCCESS;
}
