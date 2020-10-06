#include <string>
#include <vtkGenericDataObjectReader.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

int main(int argc, char* argv[])
{
  // Ensure a filename was specified
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " InputFilename e.g. blow.vtk" << endl;
    return EXIT_FAILURE;
  }

  // Get the filename from the command line
  std::string inputFilename = argv[1];

  // Get all data from the file
  vtkNew<vtkGenericDataObjectReader> reader;
  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  // All of the standard data types can be checked and obtained like this:
  if (reader->IsFilePolyData())
  {
    std::cout << "output is polydata," << std::endl;
    auto output = reader->GetPolyDataOutput();
    std::cout << "   output has " << output->GetNumberOfPoints() << " points."
              << std::endl;
  }

  if (reader->IsFileUnstructuredGrid())
  {
    std::cout << "output is unstructured grid," << std::endl;
    auto output = reader->GetUnstructuredGridOutput();
    std::cout << "   output has " << output->GetNumberOfPoints() << " points."
              << std::endl;
  }

  return EXIT_SUCCESS;
}
