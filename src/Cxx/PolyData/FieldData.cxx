#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

int main(int, char*[])
{
  vtkNew<vtkSphereSource> source;
  source->Update();

  // Extract the polydata
  vtkNew<vtkPolyData> polydata;
  polydata->ShallowCopy(source->GetOutput());

  vtkNew<vtkDoubleArray> location;

  // Create the data to store (here we just use (0,0,0))
  double locationValue[3] = {0, 0, 0};

  location->SetNumberOfComponents(3);
  location->SetName("MyDoubleArray");
  location->InsertNextTuple(locationValue);
  // The data is added to FIELD data (rather than POINT data as usual)
  polydata->GetFieldData()->AddArray(location);

  vtkNew<vtkIntArray> intValue;
  intValue->SetNumberOfComponents(1);
  intValue->SetName("MyIntValue");
  intValue->InsertNextValue(5);

  polydata->GetFieldData()->AddArray(intValue);

  // Get the data back out
  auto retrievedArray = dynamic_cast<vtkIntArray*>(
      polydata->GetFieldData()->GetAbstractArray("MyIntValue"));

  std::cout << retrievedArray->GetValue(0) << std::endl;

  return EXIT_SUCCESS;
}
