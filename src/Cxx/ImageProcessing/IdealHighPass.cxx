#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageButterworthHighPass.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageFFT.h>
#include <vtkImageIdealHighPass.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageMapper3D.h>
#include <vtkImageProperty.h>
#include <vtkImageRFFT.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{
  // Verify input arguments
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename e.g. fullhead15.png"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkImageFFT> fft;
  fft->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageIdealHighPass> idealHighPass;
  idealHighPass->SetInputConnection(fft->GetOutputPort());
  idealHighPass->SetXCutOff(0.1);
  idealHighPass->SetYCutOff(0.1);

  vtkNew<vtkImageRFFT> idealRfft;
  idealRfft->SetInputConnection(idealHighPass->GetOutputPort());

  vtkNew<vtkImageExtractComponents> idealReal;
  idealReal->SetInputConnection(idealRfft->GetOutputPort());
  idealReal->SetComponents(0);

  vtkNew<vtkImageButterworthHighPass> butterworthHighPass;
  butterworthHighPass->SetInputConnection(fft->GetOutputPort());
  butterworthHighPass->SetXCutOff(0.1);
  butterworthHighPass->SetYCutOff(0.1);

  vtkNew<vtkImageRFFT> butterworthRfft;
  butterworthRfft->SetInputConnection(butterworthHighPass->GetOutputPort());

  vtkNew<vtkImageExtractComponents> butterworthReal;
  butterworthReal->SetInputConnection(butterworthRfft->GetOutputPort());
  butterworthReal->SetComponents(0);

  // Create actors
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkImageMapToWindowLevelColors> idealColor;
  idealColor->SetWindow(500);
  idealColor->SetLevel(0);
  idealColor->SetInputConnection(idealReal->GetOutputPort());

  vtkNew<vtkImageActor> idealActor;
  idealActor->GetMapper()->SetInputConnection(idealColor->GetOutputPort());
  idealActor->GetProperty()->SetInterpolationTypeToNearest();

  vtkNew<vtkImageMapToWindowLevelColors> butterworthColor;
  butterworthColor->SetWindow(500);
  butterworthColor->SetLevel(0);
  butterworthColor->SetInputConnection(butterworthReal->GetOutputPort());

  vtkNew<vtkImageActor> butterworthActor;
  butterworthActor->GetMapper()->SetInputConnection(
      butterworthColor->GetOutputPort());
  butterworthActor->GetProperty()->SetInterpolationTypeToNearest();

  // Setup renderers
  vtkNew<vtkRenderer> idealRenderer;
  idealRenderer->SetViewport(0.0, 0.0, 0.5, 1.0);
  idealRenderer->AddActor(idealActor);
  idealRenderer->ResetCamera();
  idealRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderer> butterworthRenderer;
  butterworthRenderer->SetViewport(0.5, 0.0, 1.0, 1.0);
  butterworthRenderer->AddActor(butterworthActor);
  butterworthRenderer->SetActiveCamera(idealRenderer->GetActiveCamera());
  butterworthRenderer->SetBackground(
      colors->GetColor3d("LightSlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("IdealHighPass");
  renderWindow->AddRenderer(idealRenderer);
  renderWindow->AddRenderer(butterworthRenderer);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  idealRenderer->GetActiveCamera()->Dolly(1.4);
  idealRenderer->ResetCameraClippingRange();
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
