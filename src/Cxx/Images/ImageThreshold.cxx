#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageMapper3D.h>
#include <vtkImageThreshold.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an image
  vtkNew<vtkImageMandelbrotSource> imageSource;
  imageSource->Update();

  vtkNew<vtkImageThreshold> imageThreshold;
  imageThreshold->SetInputConnection(imageSource->GetOutputPort());
  unsigned char lower = 100;
  unsigned char upper = 200;

  imageThreshold->ThresholdBetween(lower, upper);
  imageThreshold->ReplaceInOn();
  imageThreshold->SetInValue(255);
  imageThreshold->Update();

  // Create actors
  vtkNew<vtkImageActor> inputActor;
  inputActor->GetMapper()->SetInputConnection(imageSource->GetOutputPort());

  vtkNew<vtkImageActor> thresholdedActor;
  thresholdedActor->GetMapper()->SetInputConnection(
      imageThreshold->GetOutputPort());

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("ImageThreshold");

  // And one interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(.6, .5, .4);
  leftRenderer->SetBackground(colors->GetColor3d("Sienna").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(.4, .5, .6);
  rightRenderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());

  leftRenderer->AddActor(inputActor);
  rightRenderer->AddActor(thresholdedActor);

  leftRenderer->ResetCamera();
  rightRenderer->ResetCamera();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
