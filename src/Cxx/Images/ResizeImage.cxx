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

#include <array>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkSmartPointer<vtkImageData> imageData;

  int newSize[2] = {200, 10};
  int windowFunction = 0;

  // Verify input arguments
  // e.g. Gourds2.jpg 1280 1024 5
  if (argc > 1)
  {
    // Read the image
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> reader;
    reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
    reader->SetFileName(argv[1]);
    reader->Update();

    imageData = reader->GetOutput();

    if (argc > 3)
    {
      newSize[0] = atoi(argv[2]);
      newSize[1] = atoi(argv[3]);
    }
    if (argc > 4)
    {
      windowFunction = atoi(argv[4]);
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

  vtkNew<vtkImageSincInterpolator> interpolator;
  interpolator->UseWindowParameterOn();
  if (windowFunction >= 0 && windowFunction <= 10)
  {
    interpolator->SetWindowFunction(windowFunction);
  }

  vtkNew<vtkImageResize> resize;
  resize->SetInputData(imageData);
  resize->SetInterpolator(interpolator);
  resize->SetOutputDimensions(newSize[0], newSize[1], 1);
  resize->InterpolateOn();

  if (windowFunction < 0)
  {
    resize->InterpolateOff();
    std::cout << "Using nearest neighbor interpolation" << std::endl;
    ;
  }
  else
  {
    std::cout << "Using window function : "
              << interpolator->GetWindowFunctionAsString() << std::endl;
    ;
  }

  // Create an image actor to display the image
  vtkNew<vtkImageActor> imageActor;
  imageActor->GetMapper()->SetInputConnection(resize->GetOutputPort());
  imageActor->InterpolateOff();

  // Setup renderer
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(imageActor);
  renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Dolly(5.0);
  renderer->ResetCameraClippingRange();

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(1280, 1024);
  renderWindow->SetWindowName("ResizeImage");

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
