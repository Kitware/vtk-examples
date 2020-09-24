#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageNormalize.h>
#include <vtkImageSinusoidSource.h>
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
  vtkNew<vtkImageSinusoidSource> source;
  source->Update();

  vtkNew<vtkImageNormalize> normalizeFilter;

  normalizeFilter->SetInputConnection(source->GetOutputPort());
  normalizeFilter->Update();

  vtkNew<vtkImageCast> inputCastFilter;
  inputCastFilter->SetInputConnection(source->GetOutputPort());
  inputCastFilter->SetOutputScalarTypeToUnsignedChar();
  inputCastFilter->Update();

  vtkNew<vtkImageCast> normalizeCastFilter;
  normalizeCastFilter->SetInputConnection(normalizeFilter->GetOutputPort());
  normalizeCastFilter->SetOutputScalarTypeToUnsignedChar();
  normalizeCastFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> inputActor;
  inputActor->GetMapper()->SetInputConnection(inputCastFilter->GetOutputPort());

  vtkNew<vtkImageActor> normalizedActor;
  normalizedActor->GetMapper()->SetInputConnection(
      normalizeCastFilter->GetOutputPort());

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);

  // And one interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("Sienna").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  leftRenderer->AddActor(inputActor);
  rightRenderer->AddActor(normalizedActor);

  leftRenderer->ResetCamera();

  rightRenderer->ResetCamera();

  renderWindow->SetWindowName("ImageNormalize");
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
