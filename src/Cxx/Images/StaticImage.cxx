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
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename e.g. Ox.jpg" << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);
  reader->Update();

  // Visualize
  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(reader->GetOutputPort());
  imageViewer->GetRenderWindow()->SetSize(500, 500);
  imageViewer->GetRenderWindow()->SetWindowName("StaticImage");
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("Peru").GetData());
  imageViewer->GetRenderer()->ResetCamera();

  // Set up an interactor that does not respond to mouse events
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  imageViewer->GetRenderWindow()->SetInteractor(renderWindowInteractor);
  renderWindowInteractor->SetInteractorStyle(0);
  imageViewer->Render();

  // Start the event loop
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
