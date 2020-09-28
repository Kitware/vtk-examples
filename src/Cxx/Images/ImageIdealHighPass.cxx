#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageFFT.h>
#include <vtkImageIdealHighPass.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageRFFT.h>
#include <vtkImageShiftScale.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
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
  vtkNew<vtkImageMandelbrotSource> mandelbrotSource;
  mandelbrotSource->Update();

  // Display the original image
  vtkNew<vtkImageCast> originalCastFilter;
  originalCastFilter->SetInputConnection(mandelbrotSource->GetOutputPort());
  originalCastFilter->SetOutputScalarTypeToUnsignedChar();
  originalCastFilter->Update();

  vtkNew<vtkImageSliceMapper> originalSliceMapper;
  originalSliceMapper->SetInputConnection(originalCastFilter->GetOutputPort());

  vtkNew<vtkImageSlice> originalSlice;
  originalSlice->SetMapper(originalSliceMapper);

  // Compute the FFT of the image
  vtkNew<vtkImageFFT> fftFilter;
  fftFilter->SetInputConnection(originalCastFilter->GetOutputPort());
  fftFilter->Update();

  // High pass filter the FFT
  vtkNew<vtkImageIdealHighPass> highPassFilter;
  highPassFilter->SetInputConnection(fftFilter->GetOutputPort());
  highPassFilter->SetXCutOff(.1);
  highPassFilter->SetYCutOff(.1);
  highPassFilter->Update();

  // Compute the IFFT of the high pass filtered image
  vtkNew<vtkImageRFFT> rfftFilter;
  rfftFilter->SetInputConnection(highPassFilter->GetOutputPort());
  rfftFilter->Update();

  vtkNew<vtkImageExtractComponents> extractRealFilter;
  extractRealFilter->SetInputConnection(rfftFilter->GetOutputPort());
  extractRealFilter->SetComponents(0);
  extractRealFilter->Update();

  vtkNew<vtkImageShiftScale> shiftScaleFilter;
  shiftScaleFilter->SetOutputScalarTypeToUnsignedChar();
  shiftScaleFilter->SetInputConnection(extractRealFilter->GetOutputPort());
  shiftScaleFilter->SetShift(
      -1.0f *
      extractRealFilter->GetOutput()
          ->GetScalarRange()[0]); // brings the lower bound to 0
  float oldRange = extractRealFilter->GetOutput()->GetScalarRange()[1] -
      extractRealFilter->GetOutput()->GetScalarRange()[0];
  float newRange =
      100; // We want the output [0,100] (the same as the original image)
  shiftScaleFilter->SetScale(newRange / oldRange);
  shiftScaleFilter->Update();

  // Cast the output back to unsigned char
  vtkNew<vtkImageCast> outputCastFilter;
  outputCastFilter->SetInputConnection(shiftScaleFilter->GetOutputPort());
  outputCastFilter->SetOutputScalarTypeToUnsignedChar();
  outputCastFilter->Update();

  // Display the high pass filtered image
  vtkNew<vtkImageSliceMapper> highPassSliceMapper;
  highPassSliceMapper->SetInputConnection(outputCastFilter->GetOutputPort());

  vtkNew<vtkImageSlice> highPassSlice;
  highPassSlice->SetMapper(highPassSliceMapper);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double highPassViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddViewProp(originalSlice);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(.4, .5, .6);
  originalRenderer->SetBackground(colors->GetColor3d("Sienna").GetData());

  vtkNew<vtkRenderer> highPassRenderer;
  highPassRenderer->SetViewport(highPassViewport);
  highPassRenderer->AddViewProp(highPassSlice);
  highPassRenderer->ResetCamera();
  highPassRenderer->SetBackground(.4, .5, .7);
  highPassRenderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(highPassRenderer);
  renderWindow->SetWindowName("ImageIdealHighPass");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
