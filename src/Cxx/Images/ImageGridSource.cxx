#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageGridSource.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an image
  vtkNew<vtkImageGridSource> source;
  source->SetFillValue(122);
  source->Update();

  vtkNew<vtkImageCast> castFilter;
  castFilter->SetInputConnection(source->GetOutputPort());
  castFilter->SetOutputScalarTypeToUnsignedChar();
  castFilter->Update();

  // Create an actor
  vtkNew<vtkImageActor> actor;
  actor->GetMapper()->SetInputConnection(castFilter->GetOutputPort());

  // Setup renderer
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ImageGridSource");

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
