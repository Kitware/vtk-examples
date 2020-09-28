#include <vtkActor.h>
#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageCast.h>
#include <vtkImageCorrelation.h>
#include <vtkImageData.h>
#include <vtkImageDivergence.h>
#include <vtkImageGradient.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageMapper3D.h>
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
  vtkNew<vtkImageMandelbrotSource> source;
  source->Update();

  vtkNew<vtkImageCast> originalCastFilter;
  originalCastFilter->SetInputConnection(source->GetOutputPort());
  originalCastFilter->SetOutputScalarTypeToFloat();
  originalCastFilter->Update();

  // Compute the gradient (to produce a vector field)
  vtkNew<vtkImageGradient> gradientFilter;
  gradientFilter->SetInputConnection(source->GetOutputPort());
  gradientFilter->Update();

  vtkNew<vtkImageDivergence> divergenceFilter;
  divergenceFilter->SetInputConnection(gradientFilter->GetOutputPort());
  divergenceFilter->Update();

  vtkNew<vtkImageCast> divergenceCastFilter;
  divergenceCastFilter->SetInputConnection(divergenceFilter->GetOutputPort());
  divergenceCastFilter->SetOutputScalarTypeToFloat();
  divergenceCastFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(
      originalCastFilter->GetOutputPort());

  vtkNew<vtkImageActor> divergenceActor;
  divergenceActor->GetMapper()->SetInputConnection(
      divergenceCastFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(leftViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(colors->GetColor3d("Sienna").GetData());

  vtkNew<vtkRenderer> divergenceRenderer;
  divergenceRenderer->SetViewport(rightViewport);
  divergenceRenderer->AddActor(divergenceActor);
  divergenceRenderer->ResetCamera();
  divergenceRenderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(divergenceRenderer);
  renderWindow->SetWindowName("ImageDivergence");

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
