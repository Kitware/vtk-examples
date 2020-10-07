#include <vtkNew.h>
#include <vtkImageViewer2.h>
#include <vtkPNMReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>


int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename(.pnm) e.g Gourds.pnm"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkPNMReader> reader;
  reader->SetFileName(argv[1]);

  // Visualize
  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(reader->GetOutputPort());
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  imageViewer->SetupInteractor(renderWindowInteractor);
  imageViewer->Render();
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("SlateGray").GetData());
  imageViewer->GetRenderWindow()->SetWindowName("ReadPNM");
  imageViewer->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
