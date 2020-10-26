#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkSortDataArray.h>

int main(int, char*[])
{

  vtkNew<vtkDoubleArray> valueArray;
  valueArray->InsertNextValue(20.0);
  valueArray->InsertNextValue(10.0);
  valueArray->InsertNextValue(30.0);

  vtkNew<vtkIntArray> keyArray;
  keyArray->InsertNextValue(1);
  keyArray->InsertNextValue(0);
  keyArray->InsertNextValue(2);

  std::cout << "Unsorted: " << valueArray->GetValue(0) << " "
            << valueArray->GetValue(1) << " " << valueArray->GetValue(2)
            << std::endl;

  // Sort the array
  vtkNew<vtkSortDataArray> sortDataArray;
  sortDataArray->Sort(keyArray, valueArray);

  std::cout << "Sorted: " << valueArray->GetValue(0) << " "
            << valueArray->GetValue(1) << " " << valueArray->GetValue(2)
            << std::endl;

  return EXIT_SUCCESS;
}
