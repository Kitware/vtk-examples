#include <vtkFloatArray.h>
#include <vtkIdList.h>
#include <vtkNew.h>

int main(int, char*[])
{
  vtkNew<vtkFloatArray> distances;
  distances->SetNumberOfComponents(1);
  distances->SetName("Distances");

  distances->InsertNextValue(5);
  distances->InsertNextValue(15);
  distances->InsertNextValue(15);
  distances->InsertNextValue(25);
  distances->InsertNextValue(15);

  // Get first location
  vtkIdType result = distances->LookupValue(15);
  std::cout << "result: " << result << std::endl;

  // Get all locations
  vtkNew<vtkIdList> idList;
  distances->LookupValue(15, idList);
  std::cout << "found at: " << std::endl;
  for (vtkIdType i = 0; i < idList->GetNumberOfIds(); i++)
  {
    std::cout << idList->GetId(i) << " ";
  }

  return EXIT_SUCCESS;
}
