#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMask.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGWriter.h>
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

  // Create a red image
  source->SetDrawColor(255, 0, 0);
  source->FillBox(0, 200, 0, 200);

  source->Update();

  // Create a rectanglular mask
  vtkNew<vtkImageCanvasSource2D> maskSource;
  maskSource->SetScalarTypeToUnsignedChar();
  maskSource->SetNumberOfScalarComponents(1);
  maskSource->SetExtent(0, 200, 0, 200, 0, 0);

  // Initialize the mask to black
  maskSource->SetDrawColor(0, 0, 0);
  maskSource->FillBox(0, 200, 0, 200);

  // Create a square
  maskSource->SetDrawColor(255, 255,
                           255); // anything non-zero means "make the output
                                 // pixel equal the input pixel." If the mask is
                                 // zero, the output pixel is set to MaskedValue
  maskSource->FillBox(100, 120, 100, 120);
  maskSource->Update();

  vtkNew<vtkImageMask> maskFilter;
  maskFilter->SetInputConnection(0, source->GetOutputPort());
  maskFilter->SetInputConnection(1, maskSource->GetOutputPort());
  maskFilter->SetMaskedOutputValue(0, 1, 0);
  maskFilter->Update();

  vtkNew<vtkImageMask> inverseMaskFilter;
  inverseMaskFilter->SetInputConnection(0, source->GetOutputPort());
  inverseMaskFilter->SetInputConnection(1, maskSource->GetOutputPort());
  inverseMaskFilter->SetMaskedOutputValue(0, 1, 0);
  inverseMaskFilter->NotMaskOn();
  inverseMaskFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkImageActor> maskActor;
  maskActor->GetMapper()->SetInputConnection(maskSource->GetOutputPort());

  vtkNew<vtkImageActor> maskedActor;
  maskedActor->GetMapper()->SetInputConnection(maskFilter->GetOutputPort());

  vtkNew<vtkImageActor> inverseMaskedActor;
  inverseMaskedActor->GetMapper()->SetInputConnection(
      inverseMaskFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.25, 1.0};
  double maskViewport[4] = {0.25, 0.0, 0.5, 1.0};
  double maskedViewport[4] = {0.5, 0.0, 0.75, 1.0};
  double inverseMaskedViewport[4] = {0.75, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(colors->GetColor3d("SandyBrown").GetData());

  vtkNew<vtkRenderer> maskRenderer;
  maskRenderer->SetViewport(maskViewport);
  maskRenderer->AddActor(maskActor);
  maskRenderer->ResetCamera();
  maskRenderer->SetBackground(colors->GetColor3d("Peru").GetData());

  vtkNew<vtkRenderer> maskedRenderer;
  maskedRenderer->SetViewport(maskedViewport);
  maskedRenderer->AddActor(maskedActor);
  maskedRenderer->ResetCamera();
  maskedRenderer->SetBackground(colors->GetColor3d("SandyBrown").GetData());

  vtkNew<vtkRenderer> inverseMaskedRenderer;
  inverseMaskedRenderer->SetViewport(inverseMaskedViewport);
  inverseMaskedRenderer->AddActor(inverseMaskedActor);
  inverseMaskedRenderer->ResetCamera();
  inverseMaskedRenderer->SetBackground(colors->GetColor3d("Peru").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(1000, 250);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(maskRenderer);
  renderWindow->AddRenderer(maskedRenderer);
  renderWindow->AddRenderer(inverseMaskedRenderer);
  renderWindow->SetWindowName("ImageMask");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
