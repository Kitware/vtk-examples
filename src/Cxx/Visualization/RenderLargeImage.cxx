#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkImageViewer.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderLargeImage.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0]
              << " Input(.vtp) Output(.png) [Magnification]" << std::endl;
    std::cerr << "e.g. Bunny.vtp Bunny.png 4" << std::endl;
    return EXIT_FAILURE;
  }
  int magnification = 4;
  if (argc == 4)
  {
    magnification = atoi(argv[3]);
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(argv[1]);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Tan").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("RenderLargeImage");

  renderer->AddActor(actor);

  // Let the renderer compute good position and focal point
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Dolly(1.4);
  renderer->ResetCameraClippingRange();
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  renderWindow->SetSize(640, 480);
  renderWindow->Render();

  std::cout << "Interact with image to get desired view and then press 'e'"
            << std::endl;
  interactor->Start();
  std::cout << "Generating large image size: "
            << renderWindow->GetSize()[0] * magnification << " by "
            << renderWindow->GetSize()[1] * magnification << std::endl;

  vtkNew<vtkRenderLargeImage> renderLarge;
  renderLarge->SetInput(renderer);
  renderLarge->SetMagnification(magnification);

  std::cout << "Saving image in " << argv[2] << std::endl;
  vtkNew<vtkPNGWriter> writer;
  writer->SetFileName(argv[2]);
  writer->SetInputConnection(renderLarge->GetOutputPort());
  writer->Write();

  return EXIT_SUCCESS;
}
