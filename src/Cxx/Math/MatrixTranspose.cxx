#include <vtkMatrix3x3.h>
#include <vtkNew.h>

int main(int, char*[])
{
  vtkNew<vtkMatrix3x3> m;

  m->SetElement(2, 1, 2.0); // Set element (2,1) to 2.0

  std::cout << *m << std::endl;

  m->Transpose();

  std::cout << *m << std::endl;

  return EXIT_SUCCESS;
}
