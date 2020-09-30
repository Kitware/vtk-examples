#include <vtkArrowSource.h>
#include <vtkGlyph3DMapper.h>
#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageMagnitude.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMathematics.h>
#include <vtkImageShiftScale.h>
#include <vtkImageSobel2D.h>
#include <vtkInteractorStyleImage.h>
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
  auto color2 = colors->GetColor3ub("Red").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }
  // Create an image of a rectangle
  vtkNew<vtkImageCanvasSource2D> source;
  source->SetScalarTypeToUnsignedChar();
  source->SetExtent(0, 200, 0, 200, 0, 0);
  source->SetDrawColor(drawColor1.data());
  source->FillBox(0, 200, 0, 200);
  source->SetDrawColor(drawColor2.data());
  source->FillBox(100, 120, 100, 120);
  source->Update();

  // Find the x and y gradients using a sobel filter
  vtkNew<vtkImageSobel2D> sobelFilter;
  sobelFilter->SetInputConnection(source->GetOutputPort());
  sobelFilter->Update();

  // Extract the x component of the gradient
  vtkNew<vtkImageExtractComponents> extractXFilter;
  extractXFilter->SetComponents(0);
  extractXFilter->SetInputConnection(sobelFilter->GetOutputPort());
  extractXFilter->Update();

  double xRange[2];
  extractXFilter->GetOutput()->GetPointData()->GetScalars()->GetRange(xRange);

  vtkNew<vtkImageMathematics> xImageAbs;
  xImageAbs->SetOperationToAbsoluteValue();
  xImageAbs->SetInputConnection(extractXFilter->GetOutputPort());
  xImageAbs->Update();

  vtkNew<vtkImageShiftScale> xShiftScale;
  xShiftScale->SetOutputScalarTypeToUnsignedChar();
  xShiftScale->SetScale(255 / xRange[1]);
  xShiftScale->SetInputConnection(xImageAbs->GetOutputPort());
  xShiftScale->Update();

  // Extract the y component of the gradient
  vtkNew<vtkImageExtractComponents> extractYFilter;
  extractYFilter->SetComponents(1);
  extractYFilter->SetInputConnection(sobelFilter->GetOutputPort());
  extractYFilter->Update();

  double yRange[2];
  extractYFilter->GetOutput()->GetPointData()->GetScalars()->GetRange(yRange);

  vtkNew<vtkImageMathematics> yImageAbs;
  yImageAbs->SetOperationToAbsoluteValue();
  yImageAbs->SetInputConnection(extractYFilter->GetOutputPort());
  yImageAbs->Update();

  vtkNew<vtkImageShiftScale> yShiftScale;
  yShiftScale->SetOutputScalarTypeToUnsignedChar();
  yShiftScale->SetScale(255 / yRange[1]);
  yShiftScale->SetInputConnection(yImageAbs->GetOutputPort());
  yShiftScale->Update();

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkImageActor> xActor;
  xActor->GetMapper()->SetInputConnection(xShiftScale->GetOutputPort());

  vtkNew<vtkImageActor> yActor;
  yActor->GetMapper()->SetInputConnection(yShiftScale->GetOutputPort());

  vtkNew<vtkArrowSource> arrowSource;
  sobelFilter->GetOutput()->GetPointData()->SetActiveVectors(
      "ImageScalarsGradient");

  vtkNew<vtkGlyph3DMapper> sobelMapper;
  sobelMapper->ScalingOn();
  sobelMapper->SetScaleFactor(.05);
  sobelMapper->SetSourceConnection(arrowSource->GetOutputPort());
  sobelMapper->SetInputConnection(sobelFilter->GetOutputPort());
  sobelMapper->Update();

  vtkNew<vtkActor> sobelActor;
  sobelActor->SetMapper(sobelMapper);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.25, 1.0};
  double xViewport[4] = {0.25, 0.0, 0.5, 1.0};
  double yViewport[4] = {0.5, 0.0, 0.75, 1.0};
  double sobelViewport[4] = {0.75, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(colors->GetColor3d("DodgerBlue").GetData());

  vtkNew<vtkRenderer> xRenderer;
  xRenderer->SetViewport(xViewport);
  xRenderer->AddActor(xActor);
  xRenderer->ResetCamera();
  xRenderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> yRenderer;
  yRenderer->SetViewport(yViewport);
  yRenderer->AddActor(yActor);
  yRenderer->ResetCamera();
  yRenderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> sobelRenderer;
  sobelRenderer->SetViewport(sobelViewport);
  sobelRenderer->AddActor(sobelActor);
  sobelRenderer->ResetCamera();
  sobelRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(1000, 250);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(xRenderer);
  renderWindow->AddRenderer(yRenderer);
  renderWindow->AddRenderer(sobelRenderer);
  renderWindow->SetWindowName("ImageSobel2D");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
