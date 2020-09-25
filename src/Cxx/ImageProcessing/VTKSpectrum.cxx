#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageFFT.h>
#include <vtkImageFourierCenter.h>
#include <vtkImageLogarithmicScale.h>
#include <vtkImageMagnitude.h>
#include <vtkImageMapToColors.h>
#include <vtkImageMapper3D.h>
#include <vtkImageProperty.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkWindowLevelLookupTable.h>

namespace {

void CreateImageActor(vtkImageActor* actor, double colorWindow,
                      double colorLevel);
}

int main(int argc, char* argv[])
{
  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename e.g. vtks.pgm" << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkImageFFT> fft;
  //  fft->SetFilteredAxes $VTK_IMAGE_X_AXIS $VTK_IMAGE_Y_AXIS
  fft->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageMagnitude> mag;
  mag->SetInputConnection(fft->GetOutputPort());

  vtkNew<vtkImageFourierCenter> center;
  center->SetInputConnection(mag->GetOutputPort());

  vtkNew<vtkImageLogarithmicScale> compress;
  compress->SetInputConnection(center->GetOutputPort());
  compress->SetConstant(15);
  compress->Update();

  // Create actors
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(reader->GetOutputPort());
  originalActor->GetProperty()->SetInterpolationTypeToNearest();

  vtkNew<vtkImageActor> compressedActor;
  compressedActor->GetMapper()->SetInputConnection(compress->GetOutputPort());
  compressedActor->GetProperty()->SetInterpolationTypeToNearest();
  CreateImageActor(compressedActor, 160, 120);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double compressedViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderer> compressedRenderer;
  compressedRenderer->SetViewport(compressedViewport);
  compressedRenderer->AddActor(compressedActor);
  compressedRenderer->ResetCamera();
  compressedRenderer->SetBackground(
      colors->GetColor3d("LightSlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("VTKSpectrum");
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(compressedRenderer);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

void CreateImageActor(vtkImageActor* actor, double colorWindow,
                      double colorLevel)
{
  vtkNew<vtkWindowLevelLookupTable> wlut;
  wlut->SetWindow(colorWindow);
  wlut->SetLevel(colorLevel);
  wlut->Build();

  // Map the image through the lookup table
  vtkNew<vtkImageMapToColors> color;
  color->SetLookupTable(wlut);
  color->SetInputData(actor->GetMapper()->GetInput());

  actor->GetMapper()->SetInputConnection(color->GetOutputPort());
  return;
}

} // namespace
