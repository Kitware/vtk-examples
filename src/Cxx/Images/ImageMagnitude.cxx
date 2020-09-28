#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageMagnitude.h>
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

  // Create an image of a rectangle
  vtkNew<vtkImageCanvasSource2D> source;
  source->SetScalarTypeToUnsignedChar();
  source->SetNumberOfScalarComponents(3);
  source->SetExtent(0, 200, 0, 200, 0, 0);

  // Clear the image
  source->SetDrawColor(0, 0, 0);
  source->FillBox(0, 200, 0, 200);

  // Draw a red box
  source->SetDrawColor(255, 0, 0);
  source->FillBox(100, 120, 100, 120);
  source->Update();

  vtkNew<vtkImageMagnitude> magnitudeFilter;
  magnitudeFilter->SetInputConnection(source->GetOutputPort());
  magnitudeFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkImageActor> magnitudeActor;
  magnitudeActor->GetMapper()->SetInputConnection(
      magnitudeFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double magnitudeViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(
      colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> magnitudeRenderer;
  magnitudeRenderer->SetViewport(magnitudeViewport);
  magnitudeRenderer->AddActor(magnitudeActor);
  magnitudeRenderer->ResetCamera();
  magnitudeRenderer->SetBackground(.4, .5, .7);
  magnitudeRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(magnitudeRenderer);
  renderWindow->SetWindowName("ImageMagnitude");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
