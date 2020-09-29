#include <vtkImageActor.h> // Note: this is a 3D actor (c.f. vtkImageMapper which is 2D)
#include <vtkImageMapper3D.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Parse input arguments
  if (argc != 2)
  {
    std::cout << "Required parameters: Filename e.g. Ox.jpg" << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);

  // Create an actor
  vtkNew<vtkImageActor> actor;
  actor->GetMapper()->SetInputConnection(reader->GetOutputPort());

  // Setup renderer
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("Peru").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> window;
  window->AddRenderer(renderer);
  window->SetWindowName("InteractWithImage");

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(window);

  // Setup interactor style (this is what implements the zooming, panning and
  // brightness adjustment functionality)
  vtkNew<vtkInteractorStyleImage> style;
  interactor->SetInteractorStyle(style);

  // Render and start interaction
  window->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
