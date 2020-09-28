#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageIslandRemoval2D.h>
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
  vtkNew<vtkImageCanvasSource2D> imageSource;
  imageSource->SetScalarTypeToUnsignedChar();
  imageSource->SetNumberOfScalarComponents(1);
  imageSource->SetExtent(0, 200, 0, 200, 0, 0);

  // Blank the image
  imageSource->SetDrawColor(0.0);
  imageSource->FillBox(0, 200, 0, 200);

  // Draw a small box
  imageSource->SetDrawColor(255.0);
  imageSource->FillBox(100, 105, 100, 105);

  // Draw a large box
  imageSource->SetDrawColor(255.0);
  imageSource->FillBox(150, 170, 150, 170);

  imageSource->Update();

  vtkNew<vtkImageIslandRemoval2D> islandRemovalFilter;
  islandRemovalFilter->SetAreaThreshold(50);
  islandRemovalFilter->SetIslandValue(255.0);
  islandRemovalFilter->SetReplaceValue(0.0);
  islandRemovalFilter->SetInputConnection(imageSource->GetOutputPort());
  islandRemovalFilter->Update();

  // Visualize
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(imageSource->GetOutputPort());

  vtkNew<vtkImageActor> islandRemovalActor;
  islandRemovalActor->GetMapper()->SetInputConnection(
      islandRemovalFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double islandRemovalViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(
      colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> islandRemovalRenderer;
  islandRemovalRenderer->SetViewport(islandRemovalViewport);
  islandRemovalRenderer->AddActor(islandRemovalActor);
  islandRemovalRenderer->ResetCamera();
  islandRemovalRenderer->SetBackground(
      colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(islandRemovalRenderer);
  renderWindow->SetWindowName("ImageIslandRemoval2D");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
