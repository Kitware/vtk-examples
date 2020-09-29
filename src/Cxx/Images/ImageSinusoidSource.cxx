#include <vtkImageSinusoidSource.h>
#include <vtkImageViewer2.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkImageSinusoidSource> sinusoidSource;
  sinusoidSource->Update();

  // Visualize
  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(sinusoidSource->GetOutputPort());
  imageViewer->GetRenderWindow()->SetSize(500, 500);
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("DimGray").GetData());
  imageViewer->GetRenderWindow()->SetWindowName("ImageSinusoidSource");

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
