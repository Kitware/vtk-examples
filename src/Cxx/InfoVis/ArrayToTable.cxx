#include <vtkArrayData.h>
#include <vtkArrayToTable.h>
#include <vtkDenseArray.h>
#include <vtkNew.h>
#include <vtkTable.h>

int main(int, char*[])
{
  vtkNew<vtkDenseArray<int>> array;
  array->Resize(2, 4);

  // set values
  std::cout << "There are " << array->GetExtents()[0].GetEnd() << std::endl;
  std::cout << "There are " << array->GetExtents()[1].GetEnd() << std::endl;

  for (vtkIdType i = 0; i < array->GetExtents()[0].GetEnd(); i++)
  {
    for (vtkIdType j = 0; j < array->GetExtents()[1].GetEnd(); j++)
    {
      array->SetValue(i, j, i + j);
    }
  }

  vtkNew<vtkArrayData> arrayData;
  arrayData->AddArray(array);

  vtkNew<vtkArrayToTable> arrayToTable;
  arrayToTable->SetInputData(arrayData);
  arrayToTable->Update();

  auto table = arrayToTable->GetOutput();
  table->Dump();

  return EXIT_SUCCESS;
}
