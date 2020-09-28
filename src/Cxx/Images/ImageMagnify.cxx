#include <vtkImageActor.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include <vtkImageMagnify.h>
#include <vtkImageMapper3D.h>
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
    std::cout << "Required arguments: filename.png e.g. Gourds.png"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkPNGReader> reader;
  reader->SetFileName(argv[1]);

  // Increase the dimensions of the image
  vtkNew<vtkImageMagnify> magnifyFilter;
  magnifyFilter->SetInputConnection(reader->GetOutputPort());
  magnifyFilter->SetMagnificationFactors(2, 1, 1);
  magnifyFilter->Update();

  // Adjust the spacing of the magnified image. This will stretch the
  // image
  vtkNew<vtkImageChangeInformation> changeFilter;
  changeFilter->SetInputConnection(magnifyFilter->GetOutputPort());
  changeFilter->SetSpacingScale(magnifyFilter->GetMagnificationFactors()[0],
                                magnifyFilter->GetMagnificationFactors()[1],
                                magnifyFilter->GetMagnificationFactors()[2]);

  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageActor> magnifiedActor;
  magnifiedActor->GetMapper()->SetInputConnection(
      changeFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double magnifiedViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(
      colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> magnifiedRenderer;
  magnifiedRenderer->SetViewport(magnifiedViewport);
  magnifiedRenderer->AddActor(magnifiedActor);
  magnifiedRenderer->ResetCamera();
  magnifiedRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(magnifiedRenderer);
  renderWindow->SetWindowName("ImageMagnify");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
