#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

void WindowModifiedCallback(vtkObject* caller,
                            long unsigned int vtkNotUsed(eventId),
                            void* vtkNotUsed(clientData),
                            void* vtkNotUsed(callData))
{
  std::cout << "Window modified" << std::endl;
  std::cout << caller->GetClassName() << std::endl;

  vtkRenderWindow* window = static_cast<vtkRenderWindow*>(caller);
  int* windowSize = window->GetSize();
  std::cout << "Size: " << windowSize[0] << " " << windowSize[1] << std::endl;

  if (windowSize[0] > 400)
  {
    window->SetSize(400, windowSize[1]);
    window->Render();
    window->Modified();
    window->Render();
  }
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("WindowModifiedEvent");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MistyRose").GetData());

  vtkNew<vtkCallbackCommand> m_pModifiedCallback;
  m_pModifiedCallback->SetCallback(WindowModifiedCallback);
  // m_pModifiedCallback->SetClientData(this);

  renderWindow->AddObserver(vtkCommand::ModifiedEvent, m_pModifiedCallback);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
