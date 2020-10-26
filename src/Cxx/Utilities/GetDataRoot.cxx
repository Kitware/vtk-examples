#include <vtkNew.h>
#include <vtkTesting.h>

int main(int, char *[])
{
  // Locate VTK_DATA_ROOT
  vtkNew<vtkTesting> testHelper;
  std::string dataRoot = testHelper->GetDataRoot();

  std::cout << "dataRoot: " << dataRoot << std::endl;

  return EXIT_SUCCESS;
}
