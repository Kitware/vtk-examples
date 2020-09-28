#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageLaplacian.h>
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

  // Create an image
  vtkNew<vtkImageMandelbrotSource> source;
  source->Update();

  vtkNew<vtkImageCast> castOriginal;
  castOriginal->SetInputConnection(source->GetOutputPort());
  castOriginal->SetOutputScalarTypeToFloat();
  castOriginal->Update();

  vtkNew<vtkImageLaplacian> laplacianFilter;
  laplacianFilter->SetInputConnection(source->GetOutputPort());
  laplacianFilter->Update();

  vtkNew<vtkImageCast> castLaplacian;
  castLaplacian->SetInputConnection(laplacianFilter->GetOutputPort());
  castLaplacian->SetOutputScalarTypeToFloat();
  castLaplacian->Update();

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(castOriginal->GetOutputPort());

  vtkNew<vtkImageActor> laplacianActor;
  laplacianActor->GetMapper()->SetInputConnection(
      castLaplacian->GetOutputPort());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("ImageLaplacian");

  vtkNew<vtkRenderWindowInteractor> interactor;

  vtkNew<vtkInteractorStyleImage> style;
  interactor->SetInteractorStyle(style);

  interactor->SetRenderWindow(renderWindow);

  // Setup both renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(leftViewport);
  originalRenderer->SetBackground(colors->GetColor3d("Sienna").GetData());

  vtkNew<vtkRenderer> laplacianRenderer;
  laplacianRenderer->SetViewport(rightViewport);
  laplacianRenderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());

  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(laplacianRenderer);

  originalRenderer->AddActor(originalActor);
  laplacianRenderer->AddActor(laplacianActor);

  originalRenderer->ResetCamera();
  laplacianRenderer->ResetCamera();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
