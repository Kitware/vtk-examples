#include <vtkDenseArray.h>
#include <vtkNew.h>

int main(int, char*[])
{
  vtkNew<vtkDenseArray<double>> array;

  array->Resize(5, 5);

  array->SetValue(4, 4, 5.0);

  std::cout << array->GetValue(4, 4) << std::endl;

  return EXIT_SUCCESS;
}
