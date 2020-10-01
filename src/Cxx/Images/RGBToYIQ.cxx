#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageMapper3D.h>
#include <vtkImageRGBToYIQ.h>
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

  // Verify command line arguments
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << "image e.g. Gourds2.jpg" << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);

  vtkNew<vtkImageRGBToYIQ> yiqFilter;
  yiqFilter->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageExtractComponents> extractYFilter;
  extractYFilter->SetInputConnection(yiqFilter->GetOutputPort());
  extractYFilter->SetComponents(0);

  vtkNew<vtkImageExtractComponents> extractIFilter;
  extractIFilter->SetInputConnection(yiqFilter->GetOutputPort());
  extractIFilter->SetComponents(1);

  vtkNew<vtkImageExtractComponents> extractQFilter;
  extractQFilter->SetInputConnection(yiqFilter->GetOutputPort());
  extractQFilter->SetComponents(2);

  // Create actors
  vtkNew<vtkImageActor> inputActor;
  inputActor->GetMapper()->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageActor> yActor;
  yActor->GetMapper()->SetInputConnection(extractYFilter->GetOutputPort());

  vtkNew<vtkImageActor> iActor;
  iActor->GetMapper()->SetInputConnection(extractIFilter->GetOutputPort());

  vtkNew<vtkImageActor> qActor;
  qActor->GetMapper()->SetInputConnection(extractQFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double inputViewport[4] = {0.0, 0.0, 0.25, 1.0};
  double yViewport[4] = {0.25, 0.0, 0.5, 1.0};
  double iViewport[4] = {0.5, 0.0, 0.75, 1.0};
  double qViewport[4] = {0.75, 0.0, 1.0, 1.0};

  // Shared camera
  vtkNew<vtkCamera> sharedCamera;

  // Setup renderers
  vtkNew<vtkRenderer> inputRenderer;
  inputRenderer->SetViewport(inputViewport);
  inputRenderer->AddActor(inputActor);
  inputRenderer->SetActiveCamera(sharedCamera);
  inputRenderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> yRenderer;
  yRenderer->SetViewport(yViewport);
  yRenderer->AddActor(yActor);
  yRenderer->SetActiveCamera(sharedCamera);
  yRenderer->SetBackground(colors->GetColor3d("MistyRose").GetData());

  vtkNew<vtkRenderer> iRenderer;
  iRenderer->SetViewport(iViewport);
  iRenderer->AddActor(iActor);
  iRenderer->SetActiveCamera(sharedCamera);
  iRenderer->SetBackground(colors->GetColor3d("LavenderBlush").GetData());

  vtkNew<vtkRenderer> qRenderer;
  qRenderer->SetViewport(qViewport);
  qRenderer->AddActor(qActor);
  qRenderer->SetActiveCamera(sharedCamera);
  qRenderer->SetBackground(colors->GetColor3d("Lavender").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(1000, 250);
  renderWindow->SetWindowName("RGBToYIQ");
  renderWindow->AddRenderer(inputRenderer);
  renderWindow->AddRenderer(yRenderer);
  renderWindow->AddRenderer(iRenderer);
  renderWindow->AddRenderer(qRenderer);
  inputRenderer->ResetCamera();

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
