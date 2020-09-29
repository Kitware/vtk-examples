#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageFFT.h>
#include <vtkImageMapper3D.h>
#include <vtkImageRFFT.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataWriter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an image
  vtkNew<vtkImageEllipsoidSource> source;
  source->SetWholeExtent(0, 20, 0, 20, 0, 0);
  source->SetCenter(10, 10, 0);
  source->SetRadius(3, 4, 0);
  source->Update();

  vtkNew<vtkImageFFT> fftFilter;
  fftFilter->SetInputConnection(source->GetOutputPort());
  fftFilter->Update();

  vtkNew<vtkImageCast> fftCastFilter;
  fftCastFilter->SetInputConnection(fftFilter->GetOutputPort());
  fftCastFilter->SetOutputScalarTypeToUnsignedChar();
  fftCastFilter->Update();

  vtkNew<vtkImageRFFT> rfftFilter;
  rfftFilter->SetInputConnection(fftFilter->GetOutputPort());
  rfftFilter->Update();

  vtkNew<vtkImageExtractComponents> extractRealFilter;
  extractRealFilter->SetInputConnection(rfftFilter->GetOutputPort());
  extractRealFilter->SetComponents(0);
  extractRealFilter->Update();

  vtkNew<vtkImageCast> rfftCastFilter;
  rfftCastFilter->SetInputConnection(extractRealFilter->GetOutputPort());
  rfftCastFilter->SetOutputScalarTypeToUnsignedChar();
  rfftCastFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkImageActor> fftActor;
  fftActor->GetMapper()->SetInputConnection(fftCastFilter->GetOutputPort());

  vtkNew<vtkImageActor> rfftActor;
  rfftActor->GetMapper()->SetInputConnection(rfftCastFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.33, 1.0};
  double fftViewport[4] = {0.33, 0.0, 0.66, 1.0};
  double rfftViewport[4] = {0.66, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());

  vtkNew<vtkRenderer> fftRenderer;
  fftRenderer->SetViewport(fftViewport);
  fftRenderer->AddActor(fftActor);
  fftRenderer->ResetCamera();
  fftRenderer->SetBackground(colors->GetColor3d("DodgerBlue").GetData());

  vtkNew<vtkRenderer> rfftRenderer;
  rfftRenderer->SetViewport(rfftViewport);
  rfftRenderer->AddActor(rfftActor);
  rfftRenderer->ResetCamera();
  rfftRenderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(900, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(fftRenderer);
  renderWindow->AddRenderer(rfftRenderer);
  renderWindow->SetWindowName("ImageRFFT");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
