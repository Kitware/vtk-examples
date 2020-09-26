#include <vtkImageBlend.h>
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

#include <string>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Parse input arguments
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0]
              << " Input1Filename Input2Filename  e.g. Ox.jpg Gourds2.jpg"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the images
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> imgReader1;
  imgReader1.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  imgReader1->SetFileName(argv[1]);

  vtkSmartPointer<vtkImageReader2> imgReader2;
  imgReader2.TakeReference(readerFactory->CreateImageReader2(argv[2]));
  imgReader2->SetFileName(argv[2]);

  // Combine the images (blend takes multiple connections on the 0th input port)
  vtkNew<vtkImageBlend> blend;
  blend->AddInputConnection(imgReader1->GetOutputPort());
  blend->AddInputConnection(imgReader2->GetOutputPort());
  blend->SetOpacity(0, 0.5);
  blend->SetOpacity(1, 0.5);

  // Display the result
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(blend->GetOutputPort());
  imageViewer->SetupInteractor(renderWindowInteractor);
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("Peru").GetData());
  imageViewer->GetRenderWindow()->SetWindowName("CombineImages");

  imageViewer->GetRenderer()->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
