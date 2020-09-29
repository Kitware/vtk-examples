#include <vtkActor.h>
#include <vtkDataArray.h>
#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageCast.h>
#include <vtkImageCorrelation.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageShiftScale.h>
#include <vtkInteractorStyleImage.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  std::array<double, 3> drawColor1{0, 0, 0};
  std::array<double, 3> drawColor2{0, 0, 0};
  auto color1 = colors->GetColor3ub("Black").GetData();
  auto color2 = colors->GetColor3ub("Wheat").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }

  // Create an image
  vtkNew<vtkImageCanvasSource2D> imageSource;
  imageSource->SetNumberOfScalarComponents(3);
  imageSource->SetScalarTypeToUnsignedChar();
  imageSource->SetExtent(0, 300, 0, 300, 0, 0);
  imageSource->SetDrawColor(drawColor1.data());
  imageSource->FillBox(0, 300, 0, 300);
  imageSource->SetDrawColor(drawColor2.data());
  imageSource->FillTriangle(10, 100, 190, 150, 40, 250);
  imageSource->Update();

  // Create an actor
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(imageSource->GetOutputPort());

  // Create a kernel
  vtkNew<vtkImageCanvasSource2D> kernelSource;
  kernelSource->SetNumberOfScalarComponents(3);
  kernelSource->SetScalarTypeToUnsignedChar();
  kernelSource->SetExtent(0, 30, 0, 30, 0, 0);
  kernelSource->SetDrawColor(drawColor1.data());
  kernelSource->FillBox(0, 30, 0, 30);
  kernelSource->SetDrawColor(drawColor2.data());
  kernelSource->FillTriangle(10, 1, 25, 10, 1, 5);
  kernelSource->Update();

  // Create an actor
  vtkNew<vtkImageActor> kernelActor;
  kernelActor->GetMapper()->SetInputConnection(kernelSource->GetOutputPort());

  // Compute the correlation
  vtkNew<vtkImageCorrelation> correlationFilter;
  correlationFilter->SetInputConnection(0, imageSource->GetOutputPort());
  correlationFilter->SetInputConnection(1, kernelSource->GetOutputPort());
  correlationFilter->Update();

  // At this point, corr pixels are doubles
  // So, get the scalar range
  vtkImageData* corr = correlationFilter->GetOutput();
  double corrRange[2];
  corr->GetPointData()->GetScalars()->GetRange(corrRange);

  // Rescale the correlation filter output. note that it implies
  // that minimum correlation is always zero.
  vtkNew<vtkImageShiftScale> imageScale;
  imageScale->SetInputConnection(correlationFilter->GetOutputPort());
  imageScale->SetScale(255 / corrRange[1]);
  imageScale->SetOutputScalarTypeToUnsignedChar();
  imageScale->Update();

  vtkNew<vtkImageCast> correlationCastFilter;
  correlationCastFilter->SetInputConnection(imageScale->GetOutputPort());
  correlationCastFilter->SetOutputScalarTypeToUnsignedChar();
  correlationCastFilter->Update();

  // Create an actor
  vtkNew<vtkImageActor> correlationActor;
  correlationActor->GetMapper()->SetInputConnection(
      correlationCastFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.33, 1.0};
  double kernelViewport[4] = {0.33, 0.0, 0.66, 1.0};
  double correlationViewport[4] = {0.66, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(colors->GetColor3d("Mint").GetData());

  vtkNew<vtkRenderer> kernelRenderer;
  kernelRenderer->SetViewport(kernelViewport);
  kernelRenderer->AddActor(kernelActor);
  kernelRenderer->ResetCamera();
  kernelRenderer->SetBackground(colors->GetColor3d("Mint").GetData());

  vtkNew<vtkRenderer> correlationRenderer;
  correlationRenderer->SetViewport(correlationViewport);
  correlationRenderer->AddActor(correlationActor);
  correlationRenderer->ResetCamera();
  correlationRenderer->SetBackground(colors->GetColor3d("Peacock").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(900, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(kernelRenderer);
  renderWindow->AddRenderer(correlationRenderer);
  renderWindow->SetWindowName("ImageCorrelation");

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
