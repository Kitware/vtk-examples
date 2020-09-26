#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageMapper3D.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify command line arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " InputFilename e.g. Gourds2.jpg"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkImageExtractComponents> extractRedFilter;
  extractRedFilter->SetInputConnection(reader->GetOutputPort());
  extractRedFilter->SetComponents(0);
  extractRedFilter->Update();

  vtkNew<vtkImageExtractComponents> extractGreenFilter;
  extractGreenFilter->SetInputConnection(reader->GetOutputPort());
  extractGreenFilter->SetComponents(1);
  extractGreenFilter->Update();

  vtkNew<vtkImageExtractComponents> extractBlueFilter;
  extractBlueFilter->SetInputConnection(reader->GetOutputPort());
  extractBlueFilter->SetComponents(2);
  extractBlueFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> inputActor;
  inputActor->GetMapper()->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageActor> redActor;
  redActor->GetMapper()->SetInputConnection(extractRedFilter->GetOutputPort());

  vtkNew<vtkImageActor> greenActor;
  greenActor->GetMapper()->SetInputConnection(
      extractGreenFilter->GetOutputPort());

  vtkNew<vtkImageActor> blueActor;
  blueActor->GetMapper()->SetInputConnection(
      extractBlueFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double inputViewport[4] = {0.0, 0.0, 0.25, 1.0};
  double redViewport[4] = {0.25, 0.0, 0.5, 1.0};
  double greenViewport[4] = {0.5, 0.0, 0.75, 1.0};
  double blueViewport[4] = {0.75, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> inputRenderer;
  inputRenderer->SetViewport(inputViewport);
  inputRenderer->AddActor(inputActor);
  inputRenderer->ResetCamera();
  inputRenderer->SetBackground(colors->GetColor3d("Snow").GetData());

  vtkNew<vtkRenderer> redRenderer;
  redRenderer->SetViewport(redViewport);
  redRenderer->AddActor(redActor);
  redRenderer->ResetCamera();
  redRenderer->SetBackground(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkRenderer> greenRenderer;
  greenRenderer->SetViewport(greenViewport);
  greenRenderer->AddActor(greenActor);
  greenRenderer->ResetCamera();
  greenRenderer->SetBackground(colors->GetColor3d("Mint").GetData());

  vtkNew<vtkRenderer> blueRenderer;
  blueRenderer->SetViewport(blueViewport);
  blueRenderer->AddActor(blueActor);
  blueRenderer->ResetCamera();
  blueRenderer->SetBackground(colors->GetColor3d("Peacock").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(1000, 250);
  renderWindow->AddRenderer(inputRenderer);
  renderWindow->AddRenderer(redRenderer);
  renderWindow->AddRenderer(greenRenderer);
  renderWindow->AddRenderer(blueRenderer);
  renderWindow->SetWindowName("ExtractComponents");

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
