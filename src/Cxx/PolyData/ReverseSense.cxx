#include <vtkFloatArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkReverseSense.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  auto pointNormals = dynamic_cast<vtkFloatArray*>(
      sphereSource->GetOutput()->GetPointData()->GetNormals());

  std::cout << std::endl << "Normals: " << std::endl;
  // Display the first few normals
  for (unsigned int i = 0; i < 5; i++)
  {
    double pN[3];
    pointNormals->GetTuple(i, pN);
    std::cout << "Point normal " << i << ": " << pN[0] << " " << pN[1] << " "
              << pN[2] << std::endl;
  }

  vtkNew<vtkReverseSense> reverseSense;
  reverseSense->SetInputConnection(sphereSource->GetOutputPort());
  reverseSense->ReverseNormalsOn();
  reverseSense->Update();

  auto reversedNormals = dynamic_cast<vtkFloatArray*>(
      reverseSense->GetOutput()->GetPointData()->GetNormals());

  std::cout << std::endl << "Reversed: " << std::endl;
  // Display the first few normals to verify that they are flipped
  for (unsigned int i = 0; i < 5; i++)
  {
    double pN[3];
    reversedNormals->GetTuple(i, pN);
    std::cout << "Reversed normal " << i << ": " << pN[0] << " " << pN[1] << " "
              << pN[2] << std::endl;
  }

  return EXIT_SUCCESS;
}
