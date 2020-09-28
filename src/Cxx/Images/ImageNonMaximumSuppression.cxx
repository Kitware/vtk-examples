#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageGradient.h>
#include <vtkImageGradientMagnitude.h>
#include <vtkImageMapper3D.h>
#include <vtkImageNonMaximumSuppression.h>
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

  vtkNew<vtkImageSinusoidSource> source;
  source->Update();

  vtkNew<vtkImageCast> sourceCastFilter;
  sourceCastFilter->SetOutputScalarTypeToUnsignedChar();
  sourceCastFilter->SetInputConnection(source->GetOutputPort());
  sourceCastFilter->Update();

  vtkNew<vtkImageGradient> gradientFilter;
  gradientFilter->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkImageGradientMagnitude> gradientMagnitudeFilter;
  gradientMagnitudeFilter->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkImageCast> gradientMagnitudeCastFilter;
  gradientMagnitudeCastFilter->SetOutputScalarTypeToUnsignedChar();
  gradientMagnitudeCastFilter->SetInputConnection(
      gradientMagnitudeFilter->GetOutputPort());
  gradientMagnitudeCastFilter->Update();

  vtkNew<vtkImageNonMaximumSuppression> suppressionFilter;
  suppressionFilter->SetInputConnection(
      0, gradientMagnitudeFilter->GetOutputPort());
  suppressionFilter->SetInputConnection(1, gradientFilter->GetOutputPort());
  suppressionFilter->SetDimensionality(2);
  suppressionFilter->Update();

  vtkNew<vtkImageCast> suppressionCastFilter;
  suppressionCastFilter->SetOutputScalarTypeToUnsignedChar();
  suppressionCastFilter->SetInputConnection(suppressionFilter->GetOutputPort());
  suppressionCastFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(
      sourceCastFilter->GetOutputPort());

  vtkNew<vtkImageActor> gradientMagnitudeActor;
  gradientMagnitudeActor->GetMapper()->SetInputConnection(
      gradientMagnitudeCastFilter->GetOutputPort());

  vtkNew<vtkImageActor> suppressionActor;
  suppressionActor->GetMapper()->SetInputConnection(
      suppressionCastFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.33, 1.0};
  double gradientMagnitudeViewport[4] = {0.33, 0.0, 0.66, 1.0};
  double suppressionViewport[4] = {0.66, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(colors->GetColor3d("Mint").GetData());

  vtkNew<vtkRenderer> gradientMagnitudeRenderer;
  gradientMagnitudeRenderer->SetViewport(gradientMagnitudeViewport);
  gradientMagnitudeRenderer->AddActor(gradientMagnitudeActor);
  gradientMagnitudeRenderer->ResetCamera();
  gradientMagnitudeRenderer->SetBackground(
      colors->GetColor3d("Peacock").GetData());

  vtkNew<vtkRenderer> suppressionRenderer;
  suppressionRenderer->SetViewport(suppressionViewport);
  suppressionRenderer->AddActor(suppressionActor);
  suppressionRenderer->ResetCamera();
  suppressionRenderer->SetBackground(
      colors->GetColor3d("BlanchedAlmond").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(900, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(gradientMagnitudeRenderer);
  renderWindow->AddRenderer(suppressionRenderer);
  renderWindow->SetWindowName("ImageNonMaximumSuppression");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
