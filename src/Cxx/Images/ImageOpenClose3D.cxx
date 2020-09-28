#include <vtkImageAccumulate.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageOpenClose3D.h>
#include <vtkImageThreshold.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Handle the arguments
  if (argc < 2)
  {
    std::cout << "Required arguments: filename.png e.g. Yinyang.png"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkPNGReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkImageOpenClose3D> openClose;
  openClose->SetInputConnection(reader->GetOutputPort());
  openClose->SetOpenValue(0);
  openClose->SetCloseValue(255);
  openClose->SetKernelSize(5, 5, 3);
  openClose->ReleaseDataFlagOff();
  openClose->GetOutput();
  openClose->GetCloseValue();
  openClose->GetOpenValue();

  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageActor> openCloseActor;
  openCloseActor->GetMapper()->SetInputConnection(openClose->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double openCloseViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(colors->GetColor3d("Sienna").GetData());

  vtkNew<vtkRenderer> openCloseRenderer;
  openCloseRenderer->SetViewport(openCloseViewport);
  openCloseRenderer->AddActor(openCloseActor);
  openCloseRenderer->ResetCamera();
  openCloseRenderer->SetBackground(.4, .5, .7);
  openCloseRenderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(openCloseRenderer);
  renderWindow->SetWindowName("ImageOpenClose3D");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
