#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageGradientMagnitude.h>
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
  vtkNew<vtkImageEllipsoidSource> source;
  source->SetWholeExtent(0, 20, 0, 20, 0, 0);
  source->SetCenter(10, 10, 0);
  source->SetRadius(3, 4, 0);
  source->Update();

  vtkNew<vtkImageGradientMagnitude> gradientMagnitudeFilter;
  gradientMagnitudeFilter->SetInputConnection(source->GetOutputPort());
  gradientMagnitudeFilter->Update();

  vtkNew<vtkImageCast> gradientMagnitudeCastFilter;
  gradientMagnitudeCastFilter->SetInputConnection(
      gradientMagnitudeFilter->GetOutputPort());
  gradientMagnitudeCastFilter->SetOutputScalarTypeToUnsignedChar();
  gradientMagnitudeCastFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkImageActor> gradientMagnitudeActor;
  gradientMagnitudeActor->GetMapper()->SetInputConnection(
      gradientMagnitudeCastFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double gradientMagnitudeViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(
      colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> gradientMagnitudeRenderer;
  gradientMagnitudeRenderer->SetViewport(gradientMagnitudeViewport);
  gradientMagnitudeRenderer->AddActor(gradientMagnitudeActor);
  gradientMagnitudeRenderer->ResetCamera();
  gradientMagnitudeRenderer->SetBackground(
      colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(gradientMagnitudeRenderer);
  renderWindow->SetWindowName("ImageGradientMagnitude");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
