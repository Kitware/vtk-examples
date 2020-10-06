#include <vtkImageViewer2.h>
#include <vtkJPEGReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0]
              << " Filename(.jpeg/jpg) e.g. Pileated.jpg " << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkJPEGReader> jpegReader;
  jpegReader->SetFileName(argv[1]);

  // Visualize
  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(jpegReader->GetOutputPort());
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  imageViewer->SetupInteractor(renderWindowInteractor);
  imageViewer->Render();
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("DarkSlateGray").GetData());
  imageViewer->GetRenderWindow()->SetWindowName("JPEGReader");
  imageViewer->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
