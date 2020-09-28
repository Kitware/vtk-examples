#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageMapper3D.h>
#include <vtkImageShrink3D.h>
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
  vtkNew<vtkImageEllipsoidSource> source;
  source->SetWholeExtent(0, 20, 0, 20, 0, 0);
  source->SetCenter(10, 10, 0);
  source->SetRadius(3, 4, 0);
  source->Update();

  vtkNew<vtkImageShrink3D> shrinkFilter;
  shrinkFilter->SetInputConnection(source->GetOutputPort());
  shrinkFilter->SetShrinkFactors(2, 1, 1);
  shrinkFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkImageActor> shrunkActor;
  shrunkActor->GetMapper()->SetInputConnection(shrinkFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double shrunkViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(
      colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> shrunkRenderer;
  shrunkRenderer->SetViewport(shrunkViewport);
  shrunkRenderer->AddActor(shrunkActor);
  shrunkRenderer->ResetCamera();
  shrunkRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(shrunkRenderer);
  renderWindow->SetWindowName("ImageShrink3D");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
