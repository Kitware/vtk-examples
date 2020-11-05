#include <vtkActor.h>
#include <vtkAffineRepresentation2D.h>
#include <vtkAffineWidget.h>
#include <vtkAppendPolyData.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

namespace {
class vtkAffineCallback : public vtkCommand
{
public:
  static vtkAffineCallback* New()
  {
    return new vtkAffineCallback;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*);
  vtkAffineCallback() : Actor(0), AffineRep(0)
  {
    this->Transform = vtkTransform::New();
  }
  ~vtkAffineCallback()
  {
    this->Transform->Delete();
  }
  vtkActor* Actor;
  vtkAffineRepresentation2D* AffineRep;
  vtkTransform* Transform;
};
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create two spheres: a larger one and a smaller one on top of the larger one
  // to show a reference point while rotating
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkSphereSource> sphereSource2;
  sphereSource2->SetRadius(0.075);
  sphereSource2->SetCenter(0, 0.5, 0);
  sphereSource2->Update();

  // Append the two spheres into one vtkPolyData
  vtkNew<vtkAppendPolyData> append;
  append->AddInputConnection(sphereSource->GetOutputPort());
  append->AddInputConnection(sphereSource2->GetOutputPort());

  // Create a plane centered over the larger sphere with 4x4 sub sections
  vtkNew<vtkPlaneSource> planeSource;
  planeSource->SetXResolution(4);
  planeSource->SetYResolution(4);
  planeSource->SetOrigin(-1, -1, 0);
  planeSource->SetPoint1(1, -1, 0);
  planeSource->SetPoint2(-1, 1, 0);

  // Create a mapper and actor for the plane: show it as a wireframe
  vtkNew<vtkPolyDataMapper> planeMapper;
  planeMapper->SetInputConnection(planeSource->GetOutputPort());
  vtkNew<vtkActor> planeActor;
  planeActor->SetMapper(planeMapper);
  planeActor->GetProperty()->SetRepresentationToWireframe();
  planeActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

  // Create a mapper and actor for the spheres
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(append->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Create a renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("AffineWidget");

  renderer->AddActor(actor);
  renderer->AddActor(planeActor);
  renderer->GradientBackgroundOn();
  renderer->SetBackground(colors->GetColor3d("LightSkyBlue").GetData());
  renderer->SetBackground2(colors->GetColor3d("MidnightBlue").GetData());

  // Create an interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  dynamic_cast<vtkInteractorStyleSwitch*>(
      renderWindowInteractor->GetInteractorStyle())
      ->SetCurrentStyleToTrackballCamera();

  // Create an affine widget to manipulate the actor
  // the widget currently only has a 2D representation and therefore applies
  // transforms in the X-Y plane only
  vtkNew<vtkAffineWidget> affineWidget;
  affineWidget->SetInteractor(renderWindowInteractor);
  affineWidget->CreateDefaultRepresentation();
  dynamic_cast<vtkAffineRepresentation2D*>(affineWidget->GetRepresentation())
      ->PlaceWidget(actor->GetBounds());

  vtkNew<vtkAffineCallback> affineCallback;
  affineCallback->Actor = actor;
  affineCallback->AffineRep = dynamic_cast<vtkAffineRepresentation2D*>(
      affineWidget->GetRepresentation());

  affineWidget->AddObserver(vtkCommand::InteractionEvent, affineCallback);
  affineWidget->AddObserver(vtkCommand::EndInteractionEvent, affineCallback);

  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindow->Render();
  affineWidget->On();

  // begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void vtkAffineCallback::Execute(vtkObject*, unsigned long vtkNotUsed(event),
                                void*)
{
  this->AffineRep->GetTransform(this->Transform);
  this->Actor->SetUserTransform(this->Transform);
}
} // namespace
