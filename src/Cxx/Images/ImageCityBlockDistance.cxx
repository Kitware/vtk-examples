#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageCityBlockDistance.h>
#include <vtkImageData.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageMapper3D.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageThreshold.h>
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
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " BinaryImage e.g. Yinyang.jpg" << std::endl;
    return EXIT_FAILURE;
  }

  // Read file
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);

  vtkNew<vtkImageCast> castFilter;
  castFilter->SetOutputScalarTypeToShort();
  castFilter->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageCityBlockDistance> cityBlockDistanceFilter;
  cityBlockDistanceFilter->SetInputConnection(castFilter->GetOutputPort());
  cityBlockDistanceFilter->SetDimensionality(2);
  cityBlockDistanceFilter->Update();

  // Create actors
  vtkNew<vtkImageCast> inputCastFilter;
  inputCastFilter->SetOutputScalarTypeToUnsignedChar();
  inputCastFilter->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageActor> inputActor;
  inputActor->GetMapper()->SetInputConnection(inputCastFilter->GetOutputPort());

  vtkNew<vtkImageCast> distanceCastFilter;
  distanceCastFilter->SetOutputScalarTypeToUnsignedChar();
  distanceCastFilter->SetInputConnection(
      cityBlockDistanceFilter->GetOutputPort());

  vtkNew<vtkImageActor> distanceActor;
  distanceActor->GetMapper()->SetInputConnection(
      distanceCastFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double inputViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double distanceViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> inputRenderer;
  inputRenderer->SetViewport(inputViewport);
  inputRenderer->AddActor(inputActor);
  inputRenderer->ResetCamera();
  inputRenderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> distanceRenderer;
  distanceRenderer->SetViewport(distanceViewport);
  distanceRenderer->AddActor(distanceActor);
  distanceRenderer->ResetCamera();
  distanceRenderer->SetBackground(colors->GetColor3d("LightSkyBlue").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(inputRenderer);
  renderWindow->AddRenderer(distanceRenderer);
  renderWindow->SetWindowName("ImageCityBlockDistance");

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
