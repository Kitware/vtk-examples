#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageLuminance.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

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

  vtkNew<vtkImageLuminance> luminanceFilter;
  luminanceFilter->SetInputConnection(source->GetOutputPort());
  luminanceFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkImageActor> luminanceActor;
  luminanceActor->GetMapper()->SetInputConnection(
      luminanceFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double luminanceViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(
      colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> luminanceRenderer;
  luminanceRenderer->SetViewport(luminanceViewport);
  luminanceRenderer->AddActor(luminanceActor);
  luminanceRenderer->ResetCamera();
  luminanceRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(luminanceRenderer);
  renderWindow->SetWindowName("ImageLuminance");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
