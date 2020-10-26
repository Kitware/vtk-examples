#include <vtkNew.h>
#include <vtkSparseArray.h>

int main(int, char*[])
{
  vtkNew<vtkSparseArray<double>> array;
  array->Resize(5, 1);
  array->SetNullValue(-1);

  for (unsigned int i = 0; i < 5; i++)
  {
    array->SetValue(i, 0, i);
  }

  for (unsigned int i = 0; i < 6; i++)
  {
    double val = array->GetValue(i, 0);
    std::cout << val << std::endl;
  }

  return EXIT_SUCCESS;
}
