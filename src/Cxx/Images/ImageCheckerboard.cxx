#include <vtkImageCheckerboard.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
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
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0]
              << " Input1Filename Input2Filename e.g. Ox.jpg Gourds2.jpg"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the images
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader1;
  reader1.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader1->SetFileName(argv[1]);

  vtkSmartPointer<vtkImageReader2> reader2;
  reader2.TakeReference(readerFactory->CreateImageReader2(argv[2]));
  reader2->SetFileName(argv[2]);

  vtkNew<vtkImageCheckerboard> checkerboardFilter;
  checkerboardFilter->SetInputConnection(0, reader1->GetOutputPort());
  checkerboardFilter->SetInputConnection(1, reader2->GetOutputPort());
  checkerboardFilter->SetNumberOfDivisions(3, 3, 1);

  // Visualize
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(checkerboardFilter->GetOutputPort());
  imageViewer->SetupInteractor(renderWindowInteractor);
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("Wheat").GetData());
  imageViewer->GetRenderWindow()->SetWindowName("ImageCheckerboard");
  imageViewer->GetRenderWindow()->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
