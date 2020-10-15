#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>

namespace {
void ManualMethod(vtkPolyData* input);
void AutomaticMethod(vtkPolyData* input);
} // namespace

int main(int, char*[])
{
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkPolyData* polydata = sphereSource->GetOutput();

  vtkNew<vtkDoubleArray> doubles;
  doubles->SetName("Doubles");
  doubles->SetNumberOfTuples(polydata->GetNumberOfPoints());

  polydata->GetPointData()->AddArray(doubles);

  vtkNew<vtkIntArray> ints;
  ints->SetName("Ints");
  ints->SetNumberOfTuples(polydata->GetNumberOfPoints());

  polydata->GetPointData()->AddArray(ints);

  ManualMethod(polydata);
  AutomaticMethod(polydata);

  return EXIT_SUCCESS;
}

namespace {
void ManualMethod(vtkPolyData* input)
{
  unsigned int numberOfArrays = input->GetPointData()->GetNumberOfArrays();
  std::cout << "There are " << numberOfArrays << " arrays." << std::endl;

  vtkNew<vtkPolyData> newPolyData;

  for (unsigned int i = 0; i < numberOfArrays; i++)
  {
    std::cout << "array " << i << ":" << std::endl;
    std::cout << "name: " << input->GetPointData()->GetArrayName(i)
              << std::endl;
    std::cout << "type: " << input->GetPointData()->GetArray(i)->GetDataType()
              << std::endl;
    std::cout << std::endl;

    newPolyData->GetPointData()->AddArray(input->GetPointData()->GetArray(i));
  }

  std::cout << "new polydata: " << std::endl;

  for (unsigned int i = 0; i < numberOfArrays; i++)
  {
    std::cout << "array " << i << ":" << std::endl;
    std::cout << "name: " << newPolyData->GetPointData()->GetArrayName(i)
              << std::endl;
    std::cout << "type: "
              << newPolyData->GetPointData()->GetArray(i)->GetDataType()
              << std::endl;
    std::cout << std::endl;
  }
}

void AutomaticMethod(vtkPolyData* input)
{
  unsigned int numberOfArrays = input->GetPointData()->GetNumberOfArrays();
  std::cout << "There are " << numberOfArrays << " arrays." << std::endl;

  vtkNew<vtkPolyData> newPolyData;

  newPolyData->GetPointData()->PassData(input->GetPointData());

  for (unsigned int i = 0; i < numberOfArrays; i++)
  {
    std::cout << "array " << i << ":" << std::endl;
    std::cout << "name: " << input->GetPointData()->GetArrayName(i)
              << std::endl;
    std::cout << "type: " << input->GetPointData()->GetArray(i)->GetDataType()
              << std::endl;
    std::cout << std::endl;
  }

  std::cout << "new polydata: " << std::endl;

  for (unsigned int i = 0; i < numberOfArrays; i++)
  {
    std::cout << "array " << i << ":" << std::endl;
    std::cout << "name: " << newPolyData->GetPointData()->GetArrayName(i)
              << std::endl;
    std::cout << "type: "
              << newPolyData->GetPointData()->GetArray(i)->GetDataType()
              << std::endl;
    std::cout << std::endl;
  }
}
} // namespace
