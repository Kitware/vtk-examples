#include <string>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  // Parse command line arguments
  if (argc != 3)
  {
    std::cout << "Required arguments: Filename ArrayName e.g. cowHead.vtp "
                 "Gauss_Curvature"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Get filename from command line
  std::string filename = argv[1]; // first command line argument

  // Get array name
  std::string arrayName = argv[2]; // second command line argument

  // Read the file
  vtkNew<vtkXMLPolyDataReader> reader;
  std::cout << "Reading " << filename << std::endl;
  reader->SetFileName(filename.c_str());
  reader->Update();

  // Extract the polydata
  auto polydata = reader->GetOutput();

  // Get the number of points in the polydata
  vtkIdType idNumPointsInFile = polydata->GetNumberOfPoints();

  auto array = dynamic_cast<vtkDoubleArray*>(
      polydata->GetPointData()->GetArray(arrayName.c_str()));

  if (array)
  {
    std::cout << "Got array " << arrayName << " with " << idNumPointsInFile
              << " values" << std::endl;
    for (int i = 0; i < idNumPointsInFile; i++)
    {
      // Since there could be a lot of points just print the first 10
      if (i < 10 || i == idNumPointsInFile - 1)
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
              << " does not have a PointData array named " << arrayName
              << std::endl;
  }

  return EXIT_SUCCESS;
}
