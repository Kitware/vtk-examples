#include <vtkDelaunay3D.h>
#include <vtkNew.h>
#include <vtkSphereSource.h>
#include <vtkXMLPUnstructuredGridWriter.h>

int main(int, char *[])
{
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkDelaunay3D> delaunay;
  delaunay->SetInputConnection(sphereSource->GetOutputPort());
  delaunay->Update();

  vtkNew<vtkXMLPUnstructuredGridWriter> writer;
  writer->SetInputConnection(delaunay->GetOutputPort());
  writer->SetFileName("Test.pvtu");
  writer->SetNumberOfPieces(4);
  writer->SetStartPiece(0);
  writer->SetEndPiece(3);
  writer->Update();

  return EXIT_SUCCESS;
}
