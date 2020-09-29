#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkDoubleArray.h>
#include <vtkFieldDataToAttributeDataFilter.h>
#include <vtkGlyph3D.h>
#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageGradient.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMathematics.h>
#include <vtkImageRGBToHSV.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageShiftScale.h>
#include <vtkMaskPoints.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <array>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> namedColors;
  namedColors->SetColor("Bkg", 0.2, 0.3, 0.6);

  vtkSmartPointer<vtkImageData> originalImage;
  vtkSmartPointer<vtkImageData> image;

  int onRatio = 1;
  double scaleFactor = 1.0;
  if (argc < 2)
  {
    std::array<double, 3> drawColor1{0, 0, 0};
    auto color1 = namedColors->GetColor3ub("Gray").GetData();
    std::array<double, 3> drawColor2{0, 0, 0};
    auto color2 = namedColors->GetColor3ub("Snow").GetData();
    for (auto i = 0; i < 3; ++i)
    {
      drawColor1[i] = color1[i];
      drawColor2[i] = color2[i];
    }

    // Create an image
    vtkNew<vtkImageCanvasSource2D> imageSource;
    imageSource->SetScalarTypeToDouble();
    imageSource->SetNumberOfScalarComponents(1);
    imageSource->SetExtent(0, 6, 0, 6, 0, 0);
    imageSource->SetDrawColor(drawColor1.data());
    imageSource->FillBox(0, 6, 0, 6);
    imageSource->SetDrawColor(drawColor2.data());
    imageSource->FillBox(2, 4, 2, 4);
    imageSource->Update();

    originalImage = imageSource->GetOutput();
    image = imageSource->GetOutput();

    // Use all of the points
    onRatio = 1;
    scaleFactor = .01;
  }
  else
  {
    // Read an image
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> reader;
    reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
    reader->SetFileName(argv[1]);

    // Convert to HSV and extract the Value
    vtkNew<vtkImageRGBToHSV> hsvFilter;
    hsvFilter->SetInputConnection(reader->GetOutputPort());

    vtkNew<vtkImageExtractComponents> extractValue;
    extractValue->SetInputConnection(hsvFilter->GetOutputPort());
    extractValue->SetComponents(2);
    extractValue->Update();

    image = extractValue->GetOutput();
    originalImage = reader->GetOutput();

    // Use 1% of the points
    onRatio = image->GetPointData()->GetScalars()->GetNumberOfTuples() /
        (image->GetPointData()->GetScalars()->GetNumberOfTuples() * .01);
    scaleFactor = 1.0;
  }

  // Compute the gradient of the Value
  vtkNew<vtkImageGradient> gradientFilter;
  gradientFilter->SetInputData(image);
  gradientFilter->SetDimensionality(2);
  gradientFilter->Update();

  // Extract the x component of the gradient
  vtkNew<vtkImageExtractComponents> extractXFilter;
  extractXFilter->SetComponents(0);
  extractXFilter->SetInputConnection(gradientFilter->GetOutputPort());

  double xRange[2];

  extractXFilter->Update();
  extractXFilter->GetOutput()->GetPointData()->GetScalars()->GetRange(xRange);

  // Gradient could be negative, so take the absolute value
  vtkNew<vtkImageMathematics> imageAbsX;
  imageAbsX->SetOperationToAbsoluteValue();
  imageAbsX->SetInputConnection(extractXFilter->GetOutputPort());

  // Scale the output (0,255)
  vtkNew<vtkImageShiftScale> shiftScaleX;
  shiftScaleX->SetOutputScalarTypeToUnsignedChar();
  shiftScaleX->SetScale(255 / xRange[1]);
  shiftScaleX->SetInputConnection(imageAbsX->GetOutputPort());

  // Extract the y component of the gradient
  vtkNew<vtkImageExtractComponents> extractYFilter;
  extractYFilter->SetComponents(1);
  extractYFilter->SetInputConnection(gradientFilter->GetOutputPort());

  double yRange[2];
  extractYFilter->Update();
  extractYFilter->GetOutput()->GetPointData()->GetScalars()->GetRange(yRange);

  // Gradient could be negative, so take the absolute value
  vtkNew<vtkImageMathematics> imageAbsY;
  imageAbsY->SetOperationToAbsoluteValue();
  imageAbsY->SetInputConnection(extractYFilter->GetOutputPort());

  // Scale the output (0,255)
  vtkNew<vtkImageShiftScale> shiftScaleY;
  shiftScaleY->SetOutputScalarTypeToUnsignedChar();
  shiftScaleY->SetScale(255 / yRange[1]);
  shiftScaleY->SetInputConnection(imageAbsY->GetOutputPort());

  // Create the Glyphs for the gradient
  vtkNew<vtkArrowSource> arrowSource;

  // The gradient is 2D but Glyph3D needs 3D vectors. Add a 0 z-component
  // Also, ImageGradient generates a 2-component scalar for the
  // gradient, but Glyph3D needs normals or vectors
  vtkNew<vtkDoubleArray> zeroes;
  zeroes->SetNumberOfComponents(1);
  zeroes->SetName("Zero");
  zeroes->SetNumberOfTuples(gradientFilter->GetOutput()
                                ->GetPointData()
                                ->GetScalars()
                                ->GetNumberOfTuples());
  zeroes->FillComponent(0, 0.0);
  gradientFilter->GetOutput()->GetPointData()->AddArray(zeroes);

  std::string scalarName =
      gradientFilter->GetOutput()->GetPointData()->GetScalars()->GetName();

  vtkNew<vtkFieldDataToAttributeDataFilter> scalarsToVectors;
  scalarsToVectors->SetInputConnection(gradientFilter->GetOutputPort());
  scalarsToVectors->SetInputFieldToPointDataField();
  scalarsToVectors->SetOutputAttributeDataToPointData();
  scalarsToVectors->SetVectorComponent(0, scalarName.c_str(), 0);
  scalarsToVectors->SetVectorComponent(1, scalarName.c_str(), 1);
  scalarsToVectors->SetVectorComponent(2, "Zero", 0);

  // Select a small percentage of the gradients
  vtkNew<vtkMaskPoints> maskPoints;
  maskPoints->SetInputConnection(scalarsToVectors->GetOutputPort());
  maskPoints->RandomModeOff();
  maskPoints->SetOnRatio(onRatio);

  vtkNew<vtkGlyph3D> vectorGradientGlyph;
  vectorGradientGlyph->SetSourceConnection(arrowSource->GetOutputPort());
  vectorGradientGlyph->SetInputConnection(maskPoints->GetOutputPort());
  vectorGradientGlyph->SetScaleModeToScaleByVector();
  vectorGradientGlyph->SetVectorModeToUseVector();
  vectorGradientGlyph->SetScaleFactor(scaleFactor);

  // Visualize

  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.25, 1.0};
  double xGradientViewport[4] = {0.25, 0.0, 0.5, 1.0};
  double yGradientViewport[4] = {0.5, 0.0, 0.75, 1.0};
  double vectorGradientViewport[4] = {0.75, 0.0, 1.0, 1.0};

  vtkNew<vtkPolyDataMapper> vectorGradientMapper;
  vectorGradientMapper->SetInputConnection(
      vectorGradientGlyph->GetOutputPort());
  vectorGradientMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> vectorGradientActor;
  vectorGradientActor->SetMapper(vectorGradientMapper);
  vectorGradientActor->GetProperty()->SetColor(
      namedColors->GetColor3d("Tomato").GetData());

  // Create a renderer, render window, and interactor
  // vtkNew<vtkCamera> sharedCamera;
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->SetBackground(namedColors->GetColor3d("Bkg").GetData());

  vtkNew<vtkRenderer> xGradientRenderer;
  xGradientRenderer->SetViewport(xGradientViewport);
  xGradientRenderer->SetBackground(namedColors->GetColor3d("Bkg").GetData());

  vtkNew<vtkRenderer> yGradientRenderer;
  yGradientRenderer->SetViewport(yGradientViewport);
  yGradientRenderer->SetBackground(namedColors->GetColor3d("Bkg").GetData());

  vtkNew<vtkRenderer> vectorGradientRenderer;
  vectorGradientRenderer->SetViewport(vectorGradientViewport);
  vectorGradientRenderer->SetBackground(
      namedColors->GetColor3d("Bkg").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(1000, 250);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(xGradientRenderer);
  renderWindow->AddRenderer(yGradientRenderer);
  renderWindow->AddRenderer(vectorGradientRenderer);
  renderWindow->SetWindowName("Gradient");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputData(originalImage);
  originalActor->InterpolateOff();

  vtkNew<vtkImageActor> xGradientActor;
  xGradientActor->InterpolateOff();

  xGradientActor->GetMapper()->SetInputConnection(shiftScaleX->GetOutputPort());

  vtkNew<vtkImageActor> yGradientActor;

  yGradientActor->GetMapper()->SetInputConnection(shiftScaleY->GetOutputPort());
  yGradientActor->InterpolateOff();

  // Add the actors to the scenes
  originalRenderer->AddActor(originalActor);
  xGradientRenderer->AddActor(xGradientActor);
  yGradientRenderer->AddActor(yGradientActor);
  vectorGradientRenderer->AddActor(vectorGradientActor);
  vectorGradientRenderer->AddActor(originalActor);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
