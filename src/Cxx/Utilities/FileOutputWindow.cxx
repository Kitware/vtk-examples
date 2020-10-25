#include <vtkFileOutputWindow.h>
#include <vtkNew.h>
#include <vtkXMLPolyDataReader.h>

int main(int, char*[])
{
  vtkNew<vtkFileOutputWindow> fileOutputWindow;
  fileOutputWindow->SetFileName("output.txt");

  // Note that the SetInstance function is a static member of vtkOutputWindow.
  vtkOutputWindow::SetInstance(fileOutputWindow);

  // This causes an error intentionally (file name not specified) - this error
  // will be written to the file output.txt
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->Update();

  return EXIT_SUCCESS;
}
