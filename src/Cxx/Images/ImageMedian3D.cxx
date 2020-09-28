#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMedian3D.h>
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
  vtkNew<vtkImageCanvasSource2D> imageSource;
  imageSource->SetNumberOfScalarComponents(1);
  imageSource->SetScalarTypeToUnsignedChar();
  unsigned int xmin = 0;
  unsigned int xmax = 20;
  unsigned int ymin = 0;
  unsigned int ymax = 20;
  imageSource->SetExtent(xmin, xmax, ymin, ymax, 0, 0);

  // Make the image all black
  imageSource->SetDrawColor(0.0);
  imageSource->FillBox(xmin, xmax, ymin, ymax);

  // Draw a big white square
  imageSource->SetDrawColor(255.0);
  imageSource->FillBox(5, 15, 5, 15);

  // Add some single white pixels
  imageSource->SetDrawColor(255.0);
  imageSource->FillBox(1, 1, 1, 1);
  imageSource->FillBox(17, 17, 17, 17);

  imageSource->Update();

  vtkNew<vtkImageMedian3D> medianFilter;
  medianFilter->SetInputConnection(imageSource->GetOutputPort());
  medianFilter->SetKernelSize(3, 3, 1);
  medianFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(imageSource->GetOutputPort());

  vtkNew<vtkImageActor> medianActor;
  medianActor->GetMapper()->SetInputConnection(medianFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(leftViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(
      colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> gradientMagnitudeRenderer;
  gradientMagnitudeRenderer->SetViewport(rightViewport);
  gradientMagnitudeRenderer->AddActor(medianActor);
  gradientMagnitudeRenderer->ResetCamera();
  gradientMagnitudeRenderer->SetBackground(
      colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(gradientMagnitudeRenderer);
  renderWindow->SetWindowName("ImageMedian3D");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
