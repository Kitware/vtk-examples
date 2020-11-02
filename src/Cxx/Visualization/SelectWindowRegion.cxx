#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkJPEGReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {

void SelectionChangedCallbackFunction(vtkObject* caller,
                                      long unsigned int eventId,
                                      void* clientData, void* callData);
}

int main(int argc, char* argv[])
{
  // Parse input arguments
  if (argc != 2)
  {
    std::cout << "Required parameters: Filename e.g. Ox.jpg" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  std::string inputFilename = argv[1];

  // Read the image
  vtkNew<vtkJPEGReader> jPEGReader;
  jPEGReader->SetFileName(inputFilename.c_str());
  jPEGReader->Update();

  // Create an actor
  vtkNew<vtkImageActor> actor;
  actor->GetMapper()->SetInputConnection(jPEGReader->GetOutputPort());

  // Setup the SelectionChangedEvent callback
  vtkNew<vtkCallbackCommand> selectionChangedCallback;
  selectionChangedCallback->SetCallback(SelectionChangedCallbackFunction);

  // Setup renderer
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("SelectWindowRegion");

  // Setup 2D interaction style
  vtkNew<vtkInteractorStyleRubberBand2D> style;
  style->AddObserver(vtkCommand::SelectionChangedEvent,
                     selectionChangedCallback);

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void SelectionChangedCallbackFunction(vtkObject* vtkNotUsed(caller),
                                      long unsigned int vtkNotUsed(eventId),
                                      void* vtkNotUsed(clientData),
                                      void* callData)
{
  std::cout << "Selection changed callback" << std::endl;

  unsigned int* rect = reinterpret_cast<unsigned int*>(callData);
  unsigned int pos1X = rect[0];
  unsigned int pos1Y = rect[1];
  unsigned int pos2X = rect[2];
  unsigned int pos2Y = rect[3];

  std::cout << "Start x: " << pos1X << " Start y: " << pos1Y
            << " End x: " << pos2X << " End y: " << pos2Y << std::endl;
}

/*
//You could override this, but then you have to reimplement the functionality.
//Instead, you should use an observer

void vtkInteractorStyleRubberBand2D::OnLeftButtonUp()
{
  std::cout << "LeftButtonUp!" << std::endl;

  std::cout << "Start: " << this->StartPosition[0] << " " <<
this->StartPosition[1] << std::endl; std::cout << "End: " <<
this->EndPosition[0] << " " << this->EndPosition[1] << std::endl;

  //this->Superclass.OnLeftButtonUp(); //doesn't work

  InvokeEvent(vtkCommand::EndPickEvent);
  InvokeEvent(vtkCommand::SelectionChangedEvent);

}
*/
} // namespace
