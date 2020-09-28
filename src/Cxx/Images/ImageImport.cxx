#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXMLImageDataWriter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a c-style image
  constexpr int width = 4;
  constexpr int height = 4;

  unsigned char cImage[width * height];
  unsigned char value = 0;
  for (unsigned int row = 0; row < height; ++row)
  {
    for (unsigned int col = 0; col < width; ++col)
    {
      cImage[row * width + col] = value;
      value += 10;
    }
  }

  // Convert the c-style image to a vtkImageData
  vtkNew<vtkImageImport> imageImport;
  imageImport->SetDataSpacing(1, 1, 1);
  imageImport->SetDataOrigin(0, 0, 0);
  imageImport->SetWholeExtent(0, width - 1, 0, height - 1, 0, 0);
  imageImport->SetDataExtentToWholeExtent();
  imageImport->SetDataScalarTypeToUnsignedChar();
  imageImport->SetNumberOfScalarComponents(1);
  imageImport->SetImportVoidPointer(cImage);
  imageImport->Update();

  // Create an actor
  vtkNew<vtkImageActor> actor;
  actor->SetInputData(imageImport->GetOutput());

  // Setup renderer
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("SaddleBrown").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ImageImport");

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
