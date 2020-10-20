#include <vtkAppendFilter.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>

int main(int, char*[])
{
  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5.0);
  sphereSource->Update();

  // Combine the two data sets
  vtkNew<vtkAppendFilter> appendFilter;
  appendFilter->AddInputData(sphereSource->GetOutput());
  appendFilter->Update();

  vtkNew<vtkUnstructuredGrid> unstructuredGrid;
  unstructuredGrid->ShallowCopy(appendFilter->GetOutput());

  // Write the unstructured grid
  vtkNew<vtkXMLUnstructuredGridWriter> writer;
  writer->SetFileName("UnstructuredGrid.vtu");
  writer->SetInputData(unstructuredGrid);
  writer->Write();

  return EXIT_SUCCESS;
}
