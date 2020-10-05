#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {

void SelectionChangedCallbackFunction(vtkObject* caller,
                                      long unsigned int eventId,
                                      void* clientData, void* callData);

}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("RubberBand2DObserver");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderWindow->Render();

  vtkNew<vtkCallbackCommand> selectionChangedCallback;
  selectionChangedCallback->SetCallback(SelectionChangedCallbackFunction);

  vtkNew<vtkInteractorStyleRubberBand2D> style;
  style->AddObserver(vtkCommand::SelectionChangedEvent,
                     selectionChangedCallback);
  renderWindowInteractor->SetInteractorStyle(style);

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

void SelectionChangedCallbackFunction(vtkObject* vtkNotUsed(caller),
                                      long unsigned int vtkNotUsed(eventId),
                                      void* vtkNotUsed(clientData),
                                      void* callData)
{
  std::cout << "SelectionChanged callback" << std::endl;

  unsigned int* rect = reinterpret_cast<unsigned int*>(callData);
  unsigned int pos1X = rect[0];
  unsigned int pos1Y = rect[1];
  unsigned int pos2X = rect[2];
  unsigned int pos2Y = rect[3];

  std::cout << "Start x: " << pos1X << " Start y: " << pos1Y
            << " End x: " << pos2X << " End y: " << pos2Y << std::endl;
}

} // namespace
