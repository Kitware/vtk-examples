#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {
void KeypressCallbackFunction(vtkObject* caller, long unsigned int eventId,
                              void* clientData, void* callData);
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkCubeSource> cubeSource;
  cubeSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(cubeSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkCallbackCommand> keypressCallback;
  keypressCallback->SetCallback(KeypressCallbackFunction);
  renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent,
                                      keypressCallback);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindow->SetWindowName("First Window");
  renderWindowInteractor->Start();

  std::cout << "Window 1 closed..." << std::endl;

  // Make another interactor, but use the same render window
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor2;
  renderWindowInteractor2->AddObserver(vtkCommand::KeyPressEvent,
                                       keypressCallback);

  // Note: We can change actor and background colors for the second window.
  actor->GetProperty()->SetColor(
      colors->GetColor3d("BlanchedAlmond").GetData());
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  renderWindowInteractor2->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindow->SetWindowName("Second Window");

  renderWindowInteractor2->Start();
  std::cout << "Window 2 closed. Exiting..." << std::endl;

  return EXIT_SUCCESS;
}

namespace {
void KeypressCallbackFunction(vtkObject* caller,
                              long unsigned int vtkNotUsed(eventId),
                              void* vtkNotUsed(clientData),
                              void* vtkNotUsed(callData))
{

  auto iren = static_cast<vtkRenderWindowInteractor*>(caller);
  // Close the window
  iren->GetRenderWindow()->Finalize();

  // Stop the interactor
  iren->TerminateApp();
  std::cout << "Closing window..." << std::endl;
}
} // namespace
