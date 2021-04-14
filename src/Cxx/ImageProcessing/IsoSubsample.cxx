#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageMarchingCubes.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageShrink3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{
  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename e.g FullHead.mhd"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);
  reader->Update();

  // Smoothed pipeline
  vtkNew<vtkImageGaussianSmooth> smooth;
  smooth->SetDimensionality(3);
  smooth->SetInputConnection(reader->GetOutputPort());
  smooth->SetStandardDeviations(1.75, 1.75, 0.0);
  smooth->SetRadiusFactor(2);

  vtkNew<vtkImageShrink3D> subsampleSmoothed;
  subsampleSmoothed->SetInputConnection(smooth->GetOutputPort());
  subsampleSmoothed->SetShrinkFactors(4, 4, 1);

  vtkNew<vtkImageMarchingCubes> isoSmoothed;
  isoSmoothed->SetInputConnection(smooth->GetOutputPort());
  isoSmoothed->SetValue(0, 1150);

  vtkNew<vtkPolyDataMapper> isoSmoothedMapper;
  isoSmoothedMapper->SetInputConnection(isoSmoothed->GetOutputPort());
  isoSmoothedMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> isoSmoothedActor;
  isoSmoothedActor->SetMapper(isoSmoothedMapper);
  isoSmoothedActor->GetProperty()->SetColor(
      colors->GetColor3d("Ivory").GetData());

  // Unsmoothed pipeline
  // Sub sample the data
  vtkNew<vtkImageShrink3D> subsample;
  subsample->SetInputConnection(reader->GetOutputPort());
  subsample->SetShrinkFactors(4, 4, 1);

  vtkNew<vtkImageMarchingCubes> iso;
  iso->SetInputConnection(subsample->GetOutputPort());
  iso->SetValue(0, 1150);

  vtkNew<vtkPolyDataMapper> isoMapper;
  isoMapper->SetInputConnection(iso->GetOutputPort());
  isoMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> isoActor;
  isoActor->SetMapper(isoMapper);
  isoActor->GetProperty()->SetColor(colors->GetColor3d("Ivory").GetData());

  // Rendering Pipeline
  // Setup render window, renderer, and interactor
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  vtkNew<vtkRenderer> rendererLeft;
  rendererLeft->SetViewport(leftViewport);

  vtkNew<vtkRenderer> rendererRight;
  rendererRight->SetViewport(rightViewport);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(rendererLeft);
  renderWindow->AddRenderer(rendererRight);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  rendererLeft->AddActor(isoActor);
  rendererRight->AddActor(isoSmoothedActor);

  rendererLeft->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
  rendererLeft->GetActiveCamera()->SetPosition(0.0, -1.0, 0.0);
  rendererLeft->GetActiveCamera()->SetViewUp(0.0, 0.0, -1.0);
  rendererLeft->ResetCamera();
  rendererLeft->GetActiveCamera()->Azimuth(-20.0);
  rendererLeft->GetActiveCamera()->Elevation(20.0);
  rendererLeft->ResetCameraClippingRange();

  rendererLeft->SetBackground(colors->GetColor3d("SlateGray").GetData());
  rendererRight->SetBackground(colors->GetColor3d("LightSlateGray").GetData());
  rendererRight->SetActiveCamera(rendererLeft->GetActiveCamera());

  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("IsoSubsample");
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
