#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace {

// A class not derived from vtkObjectBase
class MyClass
{
public:
  void KeypressCallbackFunction(vtkObject*,
                                long unsigned int vtkNotUsed(eventId),
                                void* vtkNotUsed(callData))
  {
    std::cout << "Caught event in MyClass" << std::endl;
  }
};

// A class that is derived from vtkObjectBase
class MyInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
  static MyInteractorStyle* New();
  vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleTrackballCamera);

  void KeypressCallbackFunction(vtkObject*,
                                long unsigned int vtkNotUsed(eventId),
                                void* vtkNotUsed(callData))
  {
    std::cout << "Caught event in MyInteractorStyle" << std::endl;
  }
};
vtkStandardNewMacro(MyInteractorStyle);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5.0);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  // Create an actor
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ObserverMemberFunction");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  MyClass myClass;
  renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, &myClass,
                                      &MyClass::KeypressCallbackFunction);

  vtkSmartPointer<MyInteractorStyle> style =
      vtkSmartPointer<MyInteractorStyle>::New();
  renderWindowInteractor->AddObserver(
      vtkCommand::KeyPressEvent, style,
      &MyInteractorStyle::KeypressCallbackFunction);

  // vtkSmartPointer<MyInteractorStyle> style2 =
  //    vtkSmartPointer<MyInteractorStyle>::New();
  // renderWindowInteractor->AddObserver(
  //    vtkCommand::KeyPressEvent, style2,
  //    &MyInteractorStyle::KeypressCallbackFunction);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
