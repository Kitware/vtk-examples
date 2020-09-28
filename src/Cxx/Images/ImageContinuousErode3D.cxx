#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageContinuousErode3D.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkImageData> image;

  if (argc < 2)
  {
    std::array<double, 3> drawColor1{0, 0, 0};
    std::array<double, 3> drawColor2{0, 0, 0};
    auto color1 = colors->GetColor3ub("Black").GetData();
    auto color2 = colors->GetColor3ub("Red").GetData();
    for (auto i = 0; i < 3; ++i)
    {
      drawColor1[i] = color1[i];
      drawColor2[i] = color2[i];
    }

    // Create an image
    vtkNew<vtkImageCanvasSource2D> source;
    source->SetScalarTypeToUnsignedChar();
    source->SetExtent(0, 200, 0, 200, 0, 0);
    source->SetDrawColor(drawColor1.data());
    source->FillBox(0, 200, 0, 200);
    source->SetDrawColor(drawColor2.data());
    source->FillBox(100, 150, 100, 150);
    source->Update();
    image->ShallowCopy(source->GetOutput());
  }
  else
  {
    vtkNew<vtkPNGReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();
    image->ShallowCopy(reader->GetOutput());
  }

  vtkNew<vtkImageContinuousErode3D> erodeFilter;
  erodeFilter->SetInputData(image);
  erodeFilter->SetKernelSize(10, 10, 1);
  erodeFilter->Update();

  vtkNew<vtkDataSetMapper> originalMapper;
  originalMapper->SetInputData(image);
  originalMapper->Update();

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);

  vtkNew<vtkDataSetMapper> erodedMapper;
  erodedMapper->SetInputConnection(erodeFilter->GetOutputPort());
  erodedMapper->Update();

  vtkNew<vtkActor> erodedActor;
  erodedActor->SetMapper(erodedMapper);

  // Visualize
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("ImageContinuousErode3D");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("Sienna").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());

  leftRenderer->AddActor(originalActor);
  rightRenderer->AddActor(erodedActor);

  leftRenderer->ResetCamera();
  rightRenderer->SetActiveCamera(leftRenderer->GetActiveCamera());

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
