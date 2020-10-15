#include <vtkCleanPolyData.h>
#include <vtkCubeSource.h>
#include <vtkNew.h>
#include <vtkPolyData.h>

int main(int, char*[])
{
  vtkNew<vtkCubeSource> cubeSource;
  cubeSource->Update();

  std::cout << "Input cube has " << cubeSource->GetOutput()->GetNumberOfPoints()
            << " vertices." << std::endl;

  vtkNew<vtkCleanPolyData> cleanPolyData;
  cleanPolyData->SetInputConnection(cubeSource->GetOutputPort());
  cleanPolyData->Update();

  std::cout << "Cleaned cube has "
            << cleanPolyData->GetOutput()->GetNumberOfPoints() << " vertices."
            << std::endl;

  return EXIT_SUCCESS;
}
