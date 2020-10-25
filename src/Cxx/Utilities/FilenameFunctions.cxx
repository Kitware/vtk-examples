#include <string>
#include <vtkDirectory.h>
#include <vtkNew.h>
#include <vtksys/SystemTools.hxx>

int main(int argc, char* argv[])
{
  std::string directoryName;
  if (argc < 2)
  {
    directoryName = std::string(".");
  }
  else
  {
    directoryName = std::string(argv[1]);
  }

  vtkNew<vtkDirectory> directory;
  int opened = directory->Open(directoryName.c_str());

  if (!opened)
  {
    std::cout << "Invalid directory!" << std::endl;
    return EXIT_FAILURE;
  }

  int numberOfFiles = directory->GetNumberOfFiles();
  std::cout << "Number of files: " << numberOfFiles << std::endl;

  for (int i = 0; i < numberOfFiles; i++)
  {
    std::string fileString = directoryName;
    fileString += "/";
    fileString += directory->GetFile(i);

    std::string ext = vtksys::SystemTools::GetFilenameLastExtension(fileString);
    std::cout << fileString << " extension: " << ext << std::endl;

    std::string name =
        vtksys::SystemTools::GetFilenameWithoutLastExtension(fileString);
    std::cout << "name: " << name << std::endl;
  }

  return EXIT_SUCCESS;
}
