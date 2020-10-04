#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace {

void ClickCallbackFunction(vtkObject* caller, long unsigned int eventId,
                           void* clientData, void* callData);

}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5.0);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->AddActor(actor);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("MouseEventsObserver");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkCallbackCommand> clickCallback;
  clickCallback->SetCallback(ClickCallbackFunction);

  renderWindowInteractor->AddObserver(vtkCommand::LeftButtonPressEvent,
                                      clickCallback);
  renderWindowInteractor->AddObserver(vtkCommand::RightButtonPressEvent,
                                      clickCallback);
  renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, clickCallback);

  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

void ClickCallbackFunction(vtkObject* vtkNotUsed(caller),
                           long unsigned int eventId,
                           void* vtkNotUsed(clientData),
                           void* vtkNotUsed(callData))
{
  std::cout << "Click callback" << std::endl;
  std::cout << "Event: " << vtkCommand::GetStringFromEventId(eventId)
            << std::endl;

  // Get the interactor like this:
  // auto* iren = static_cast<vtkRenderWindowInteractor*>(caller);
}

} // namespace
