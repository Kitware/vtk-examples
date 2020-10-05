#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename(.img) e.g. prostate.img"
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string inputFilename = argv[1];

  // Read all the DICOM files in the specified directory.
  vtkNew<vtkDICOMImageReader> reader;
  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  // Visualize
  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(reader->GetOutputPort());
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  imageViewer->SetupInteractor(renderWindowInteractor);
  imageViewer->Render();
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("SlateGray").GetData());
  imageViewer->GetRenderWindow()->SetWindowName("ReadDICOM");
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
