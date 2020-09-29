#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an image
  vtkNew<vtkImageEllipsoidSource> source;
  source->SetWholeExtent(0, 20, 0, 20, 0, 0);
  source->SetCenter(10, 10, 0);
  source->SetRadius(3, 4, 5);
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
  renderer->SetBackground(colors->GetColor3d("Sienna").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ImageEllipsoidSource");

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
