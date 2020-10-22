#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkRectilinearGrid.h>
#include <vtkRectilinearGridToTetrahedra.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>

int main(int, char*[])
{
  // Create a grid
  vtkNew<vtkRectilinearGrid> grid;

  grid->SetDimensions(5, 4, 3);

  vtkNew<vtkDoubleArray> xArray;
  xArray->InsertNextValue(0.0);
  xArray->InsertNextValue(1.0);
  xArray->InsertNextValue(2.0);
  xArray->InsertNextValue(3.0);
  xArray->InsertNextValue(4.0);

  vtkNew<vtkDoubleArray> yArray;
  yArray->InsertNextValue(0.0);
  yArray->InsertNextValue(1.0);
  yArray->InsertNextValue(2.0);
  yArray->InsertNextValue(3.0);

  vtkNew<vtkDoubleArray> zArray;
  zArray->InsertNextValue(0.0);
  zArray->InsertNextValue(1.0);
  zArray->InsertNextValue(2.0);

  grid->SetXCoordinates(xArray);
  grid->SetYCoordinates(yArray);
  grid->SetZCoordinates(zArray);

  vtkNew<vtkRectilinearGridToTetrahedra> rectilinearGridToTetrahedra;
  rectilinearGridToTetrahedra->SetInputData(grid);
  rectilinearGridToTetrahedra->Update();

  vtkNew<vtkXMLUnstructuredGridWriter> writer;
  writer->SetFileName("output.vtu");
  writer->SetInputConnection(rectilinearGridToTetrahedra->GetOutputPort());
  writer->Write();

  std::cout << "There are "
            << rectilinearGridToTetrahedra->GetOutput()->GetNumberOfCells()
            << " cells." << std::endl;

  return EXIT_SUCCESS;
}
