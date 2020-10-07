#include <vtkImageViewer2.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTIFFReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename(.tif) e.g. ColorCells.tif"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkTIFFReader> reader;
  reader->SetFileName(argv[1]);

  // Visualize
  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(reader->GetOutputPort());
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  imageViewer->SetupInteractor(renderWindowInteractor);
  imageViewer->Render();
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("DarkSlateGray").GetData());
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->GetRenderWindow()->SetWindowName("ReadTIFF");
  imageViewer->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
