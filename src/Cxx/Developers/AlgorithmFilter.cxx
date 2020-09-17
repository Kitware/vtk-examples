#include <vtkNew.h>

#include "vtkTest.h"
#include "vtkTestAlgorithmFilter.h"

int main(int, char*[])
{
  vtkNew<vtkTest> inputTest;
  inputTest->SetValue(5.6);
  std::cout << "Input value: " << inputTest->GetValue() << std::endl;

  vtkNew<vtkTestAlgorithmFilter> filter;
  filter->SetInput(inputTest);
  filter->Update();

  vtkTest* outputTest = filter->GetOutput();
  std::cout << "Output value: " << outputTest->GetValue() << std::endl;
  std::cout << "Input value is still: " << inputTest->GetValue() << std::endl;

  return EXIT_SUCCESS;
}
