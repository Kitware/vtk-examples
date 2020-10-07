#include <vtkMatrix3x3.h>
#include <vtkNew.h>

int main(int, char*[])
{
  vtkNew<vtkMatrix3x3> m;

  m->SetElement(2, 1, 2.0); // Set element (0,0) to 1.0

  std::cout << *m << std::endl;

  m->Invert();

  std::cout << *m << std::endl;

  return EXIT_SUCCESS;
}
