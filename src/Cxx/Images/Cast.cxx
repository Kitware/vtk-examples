#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMandelbrotSource.h>
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

  // Create a float image
  vtkNew<vtkImageMandelbrotSource> source;
  source->Update();

  std::cout << source->GetOutput()->GetScalarTypeAsString() << std::endl;

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
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  renderer->ResetCamera();

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Cast");

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
