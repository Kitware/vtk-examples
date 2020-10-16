#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkTriangle.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  // Parse command line arguments
  if (argc != 3)
  {
    std::cout << "Required arguments: Filename ArrayName e.g. SuperQuadric.vtp "
                 "Normals"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Get filename from command line
  std::string filename = argv[1]; // first command line argument

  // Get array name
  std::string arrayName = argv[2]; // second command line argument

  // read the file
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();

  auto polydata = reader->GetOutput();

  // Get the number of cells in the polydata
  vtkIdType idNumCellsInFile = polydata->GetNumberOfCells();

  auto array = dynamic_cast<vtkFloatArray*>(
      polydata->GetCellData()->GetArray(arrayName.c_str()));

  if (array)
  {
    std::cout << "Got array " << arrayName << " with " << idNumCellsInFile
              << " values" << std::endl;
    for (int i = 0; i < idNumCellsInFile; i++)
    {
      // Since there could be a lot of cells just print the first 10
      if (i < 10 || i == idNumCellsInFile - 1)
      {
        double value;
        value = array->GetValue(i);
        std::cout << i << ": " << value << std::endl;
      }
      else
      {
        if (i == 10)
          std::cout << "..." << std::endl;
      }
    }
  }
  else
  {
    std::cout << "The file " << filename
              << " does not have a CellData array named " << arrayName
              << std::endl;
  }

  return EXIT_SUCCESS;
}
