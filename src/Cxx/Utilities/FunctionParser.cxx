#include <vtkFunctionParser.h>
#include <vtkNew.h>

int main(int, char*[])
{
  vtkNew<vtkFunctionParser> functionParser;
  functionParser->SetFunction("a+b");

  functionParser->SetScalarVariableValue("a", 2);
  functionParser->SetScalarVariableValue("b", 3);

  double result = functionParser->GetScalarResult();

  std::cout << "result: " << result << std::endl;

  return EXIT_SUCCESS;
}
