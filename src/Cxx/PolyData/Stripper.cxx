#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkStripper.h>

int main(int, char*[])
{
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  std::cout << "Number of cells before stripping: "
            << sphereSource->GetOutput()->GetNumberOfCells() << std::endl;

  vtkNew<vtkStripper> stripper;
  stripper->SetInputConnection(sphereSource->GetOutputPort());
  stripper->Update();

  std::cout << "Number of cells after stripping: "
            << stripper->GetOutput()->GetNumberOfCells() << std::endl;

  return EXIT_SUCCESS;
}
