#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageMapper3D.h>
#include <vtkImageSinusoidSource.h>
#include <vtkImageWeightedSum.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // create an image
  vtkNew<vtkImageMandelbrotSource> source1;
  source1->SetWholeExtent(0, 255, 0, 255, 0, 0);
  source1->Update();

  vtkNew<vtkImageCast> source1Double;
  source1Double->SetInputConnection(0, source1->GetOutputPort());
  source1Double->SetOutputScalarTypeToDouble();

  vtkNew<vtkImageSinusoidSource> source2;
  source2->SetWholeExtent(0, 255, 0, 255, 0, 0);
  source2->Update();

  vtkNew<vtkImageWeightedSum> sumFilter;
  sumFilter->SetWeight(0, .8);
  sumFilter->SetWeight(1, .2);
  sumFilter->AddInputConnection(source1Double->GetOutputPort());
  sumFilter->AddInputConnection(source2->GetOutputPort());

  sumFilter->Update();

  vtkNew<vtkImageCast> source1CastFilter;
  source1CastFilter->SetInputConnection(source1->GetOutputPort());
  source1CastFilter->SetOutputScalarTypeToUnsignedChar();
  source1CastFilter->Update();

  vtkNew<vtkImageCast> source2CastFilter;
  source2CastFilter->SetInputConnection(source2->GetOutputPort());
  source2CastFilter->SetOutputScalarTypeToUnsignedChar();
  source2CastFilter->Update();

  vtkNew<vtkImageCast> summedCastFilter;
  summedCastFilter->SetInputConnection(sumFilter->GetOutputPort());
  summedCastFilter->SetOutputScalarTypeToUnsignedChar();
  summedCastFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> source1Actor;
  source1Actor->GetMapper()->SetInputConnection(
      source1CastFilter->GetOutputPort());

  vtkNew<vtkImageActor> source2Actor;
  source2Actor->GetMapper()->SetInputConnection(
      source2CastFilter->GetOutputPort());

  vtkNew<vtkImageActor> summedActor;
  summedActor->GetMapper()->SetInputConnection(
      summedCastFilter->GetOutputPort());

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);

  // And one interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.33, 1.0};
  double centerViewport[4] = {0.33, 0.0, .66, 1.0};
  double rightViewport[4] = {0.66, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("Peru").GetData());

  vtkNew<vtkRenderer> centerRenderer;
  renderWindow->AddRenderer(centerRenderer);
  centerRenderer->SetViewport(centerViewport);
  centerRenderer->SetBackground(colors->GetColor3d("DarkTurquoise").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  leftRenderer->AddActor(source1Actor);
  centerRenderer->AddActor(source2Actor);
  rightRenderer->AddActor(summedActor);

  leftRenderer->ResetCamera();
  centerRenderer->ResetCamera();
  rightRenderer->ResetCamera();

  renderWindow->SetWindowName("ImageWeightedSum");
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
