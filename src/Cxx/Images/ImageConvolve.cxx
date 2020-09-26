#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageConvolve.h>
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

  // Create an image
  vtkNew<vtkImageMandelbrotSource> source;
  source->Update();

  vtkNew<vtkImageCast> originalCastFilter;
  originalCastFilter->SetInputConnection(source->GetOutputPort());
  originalCastFilter->SetOutputScalarTypeToUnsignedChar();
  originalCastFilter->Update();

  vtkNew<vtkImageConvolve> convolveFilter;
  convolveFilter->SetInputConnection(source->GetOutputPort());
  double kernel[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  convolveFilter->SetKernel3x3(kernel);
  convolveFilter->Update();

  vtkNew<vtkImageCast> convolvedCastFilter;
  convolvedCastFilter->SetInputConnection(convolveFilter->GetOutputPort());
  convolvedCastFilter->SetOutputScalarTypeToUnsignedChar();
  convolvedCastFilter->Update();

  // Create an actor
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(
      originalCastFilter->GetOutputPort());

  vtkNew<vtkImageActor> convolvedActor;
  convolvedActor->GetMapper()->SetInputConnection(
      convolvedCastFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderer
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(leftViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->SetBackground(colors->GetColor3d("Sienna").GetData());
  originalRenderer->ResetCamera();

  vtkNew<vtkRenderer> convolvedRenderer;
  convolvedRenderer->SetViewport(rightViewport);
  convolvedRenderer->AddActor(convolvedActor);
  convolvedRenderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());
  convolvedRenderer->ResetCamera();

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(convolvedRenderer);
  renderWindow->SetWindowName("ImageConvolve");

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
