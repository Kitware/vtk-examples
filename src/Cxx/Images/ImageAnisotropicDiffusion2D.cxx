#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageAnisotropicDiffusion2D.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageMapper3D.h>
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
  vtkNew<vtkNamedColors> colors;

  // Handle the arguments
  if (argc < 2)
  {
    std::cout
        << "Usage: " << argv[0]
        << " ImagFilename e.g cake_easy.jpg [iterations(10)] [threshold(20)]"
        << std::endl;
    return EXIT_FAILURE;
  }
  int iterations = 10;
  if (argc >= 3)
  {
    iterations = atoi(argv[2]);
  }
  float threshold = 20;
  if (argc >= 4)
  {
    threshold = atoi(argv[3]);
  }
  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);

  vtkNew<vtkImageAnisotropicDiffusion2D> diffusion;
  diffusion->SetInputConnection(reader->GetOutputPort());
  diffusion->SetNumberOfIterations(iterations);
  diffusion->SetDiffusionThreshold(threshold);
  diffusion->Update();

  // Create an actor
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(reader->GetOutputPort());

  // Create an actor
  vtkNew<vtkImageActor> diffusionActor;
  diffusionActor->GetMapper()->SetInputConnection(diffusion->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkCamera> camera;
  vtkNew<vtkRenderer> leftRenderer;
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->AddActor(originalActor);
  leftRenderer->SetActiveCamera(camera);
  leftRenderer->ResetCamera();
  leftRenderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->AddActor(diffusionActor);
  rightRenderer->SetActiveCamera(camera);
  rightRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(leftRenderer);
  renderWindow->AddRenderer(rightRenderer);
  renderWindow->SetWindowName("ImageAnisotropicDiffusion2D");

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
