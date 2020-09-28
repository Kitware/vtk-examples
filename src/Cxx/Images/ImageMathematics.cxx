#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMathematics.h>
#include <vtkInteractorStyleImage.h>
#include <vtkMath.h>
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
  imageSource->SetNumberOfScalarComponents(3);
  imageSource->SetScalarTypeToUnsignedChar();
  imageSource->SetExtent(0, 4, 0, 4, 0, 0);
  imageSource->SetDrawColor(100.0, 0, 0);
  imageSource->FillBox(0, 4, 0, 4);
  imageSource->Update();

  vtkNew<vtkImageMathematics> imageMath;
  imageMath->SetOperationToMultiplyByK();
  imageMath->SetConstantK(2.0);
  imageMath->SetInputConnection(imageSource->GetOutputPort());
  imageMath->Update();

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(imageSource->GetOutputPort());

  vtkNew<vtkImageActor> mathActor;
  mathActor->GetMapper()->SetInputConnection(imageMath->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(leftViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(
      colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> mathRenderer;
  mathRenderer->SetViewport(rightViewport);
  mathRenderer->AddActor(mathActor);
  mathRenderer->ResetCamera();
  mathRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(mathRenderer);
  renderWindow->SetWindowName("ImageMathematics");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkInteractorStyleImage> style;
  renderWindowInteractor->SetInteractorStyle(style);

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
