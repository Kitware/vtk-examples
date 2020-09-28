#include <vtkFloatArray.h>
#include <vtkImageAccumulate.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageSeparableConvolution.h>
#include <vtkImageThreshold.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Handle the arguments
  if (argc < 2)
  {
    std::cout << "Required arguments: BinaryImage.png e.g. Yinyang.png"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkPNGReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkFloatArray> xKernel;
  xKernel->SetNumberOfTuples(5);
  xKernel->SetNumberOfComponents(1);
  xKernel->SetValue(0, 1);
  xKernel->SetValue(1, 1);
  xKernel->SetValue(2, 1);
  xKernel->SetValue(3, 1);
  xKernel->SetValue(4, 1);

  vtkNew<vtkImageSeparableConvolution> convolutionFilter;
  convolutionFilter->SetInputConnection(reader->GetOutputPort());
  convolutionFilter->SetXKernel(xKernel);
  convolutionFilter->Update();

  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageCast> convolutionCastFilter;
  convolutionCastFilter->SetInputConnection(convolutionFilter->GetOutputPort());
  convolutionCastFilter->SetOutputScalarTypeToUnsignedChar();
  convolutionCastFilter->Update();

  vtkNew<vtkImageActor> convolutionActor;
  convolutionActor->GetMapper()->SetInputConnection(
      convolutionCastFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double convolutionViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(
      colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> convolutionRenderer;
  convolutionRenderer->SetViewport(convolutionViewport);
  convolutionRenderer->AddActor(convolutionActor);
  convolutionRenderer->ResetCamera();
  convolutionRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(convolutionRenderer);
  renderWindow->SetWindowName("ImageSeparableConvolution");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
