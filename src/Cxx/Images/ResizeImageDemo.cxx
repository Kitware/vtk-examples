#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageResize.h>
#include <vtkImageSincInterpolator.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

#include <array>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkSmartPointer<vtkImageData> imageData;

  double factor = .5;
  int newSize[2];

  // Verify input arguments
  if (argc > 1)
  {
    // Read the image
    // e.g. Pileated.jpg 3
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> reader;
    reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
    reader->SetFileName(argv[1]);
    reader->Update();

    imageData = reader->GetOutput();
    if (argc > 2)
    {
      factor = atof(argv[2]);
    }
  }
  else
  {
    std::array<double, 3> drawColor1{0, 0, 0};
    auto color1 = colors->GetColor3ub("Gray").GetData();
    std::array<double, 3> drawColor2{0, 0, 0};
    auto color2 = colors->GetColor3ub("Aquamarine").GetData();
    std::array<double, 3> drawColor3{0, 0, 0};
    auto color3 = colors->GetColor3ub("Violet").GetData();
    for (auto i = 0; i < 3; ++i)
    {
      drawColor1[i] = color1[i];
      drawColor2[i] = color2[i];
      drawColor3[i] = color3[i];
    }

    vtkNew<vtkImageCanvasSource2D> canvasSource;
    canvasSource->SetExtent(0, 100, 0, 100, 0, 0);
    canvasSource->SetScalarTypeToUnsignedChar();
    canvasSource->SetNumberOfScalarComponents(3);
    canvasSource->SetDrawColor(drawColor1.data());
    canvasSource->FillBox(0, 100, 0, 100);
    canvasSource->SetDrawColor(drawColor2.data());
    canvasSource->FillTriangle(10, 10, 25, 10, 25, 25);
    canvasSource->SetDrawColor(drawColor3.data());
    canvasSource->FillTube(75, 75, 0, 75, 5.0);
    canvasSource->Update();
    imageData = canvasSource->GetOutput();
  }

  std::cout << "Original dimensions: " << imageData->GetDimensions()[0] << ", "
            << imageData->GetDimensions()[1] << std::endl;
  newSize[0] = imageData->GetDimensions()[0] * factor;
  newSize[1] = imageData->GetDimensions()[1] * factor;
  std::cout << "New dimensions: " << newSize[0] << ", " << newSize[1]
            << std::endl;

  // One camera for all
  vtkNew<vtkCamera> camera;

  // Create one text property for all
  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetFontSize(20);
  textProperty->SetJustificationToCentered();
  textProperty->SetColor(colors->GetColor3d("MidnightBlue").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("ResizeImageDemo");

  std::vector<vtkSmartPointer<vtkRenderer>> renderers;
  for (int i = -1; i <= 10; ++i)
  {
    vtkNew<vtkImageSincInterpolator> interpolator;
    auto windowFunction = i;
    interpolator->UseWindowParameterOn();

    vtkNew<vtkImageResize> resize;
    resize->SetInputData(imageData);
    resize->SetInterpolator(interpolator);
    resize->SetOutputDimensions(newSize[0], newSize[1], 1);
    resize->InterpolateOn();

    // Create an image actor to display the image
    vtkNew<vtkImageActor> imageActor;
    imageActor->GetMapper()->SetInputConnection(resize->GetOutputPort());
    imageActor->InterpolateOff();

    // Create textActors
    vtkNew<vtkTextMapper> textMapper;
    textMapper->SetTextProperty(textProperty);
    if (windowFunction < 0)
    {
      resize->InterpolateOff();
      textMapper->SetInput("Nearest neighbor");
    }
    else
    {
      resize->InterpolateOn();
      interpolator->SetWindowFunction(windowFunction);
      textMapper->SetInput(interpolator->GetWindowFunctionAsString());
    }

    vtkNew<vtkActor2D> textActor;
    textActor->SetMapper(textMapper);
    textActor->SetPosition(100, 16);
    // Setup renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(imageActor);
    renderer->AddActor(textActor);
    ;
    renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Dolly(5);
    renderer->ResetCameraClippingRange();
    renderers.push_back(renderer);
    renderWindow->AddRenderer(renderer);
  }
  // Setup viewports for the renderers
  int rendererSize = 200;
  unsigned int xGridDimensions = 4;
  unsigned int yGridDimensions = 3;
  renderWindow->SetSize(rendererSize * xGridDimensions,
                        rendererSize * yGridDimensions);
  for (int row = 0; row < static_cast<int>(yGridDimensions); row++)
  {
    for (int col = 0; col < static_cast<int>(xGridDimensions); col++)
    {
      int index = row * xGridDimensions + col;

      // (xmin, ymin, xmax, ymax)
      double viewport[4] = {static_cast<double>(col) * rendererSize /
                                (xGridDimensions * rendererSize),
                            static_cast<double>(yGridDimensions - (row + 1)) *
                                rendererSize / (yGridDimensions * rendererSize),
                            static_cast<double>(col + 1) * rendererSize /
                                (xGridDimensions * rendererSize),
                            static_cast<double>(yGridDimensions - row) *
                                rendererSize /
                                (yGridDimensions * rendererSize)};
      renderers[index]->SetViewport(viewport);
    }
  }

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
