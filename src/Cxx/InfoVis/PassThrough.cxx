#include <vtkNew.h>
#include <vtkPassThrough.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  std::cout << "Points before: "
            << sphereSource->GetOutput()->GetNumberOfPoints() << std::endl;

  vtkNew<vtkPassThrough> passThrough;
  passThrough->SetInputConnection(sphereSource->GetOutputPort());
  passThrough->Update();

  auto output = dynamic_cast<vtkPolyData*>(passThrough->GetOutput());

  std::cout << "Points after: " << output->GetNumberOfPoints() << std::endl;

  return EXIT_SUCCESS;
}
