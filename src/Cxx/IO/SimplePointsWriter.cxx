#include <vtkNew.h>
#include <vtkSimplePointsWriter.h>
#include <vtkSphereSource.h>

int main(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkSimplePointsWriter> writer;
  writer->SetFileName("test.xyz");
  writer->SetInputConnection(sphereSource->GetOutputPort());
  writer->Write();

  return EXIT_SUCCESS;
}
