#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkClipPolyData.h>
#include <vtkCommand.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

namespace {
// Callback for the interaction
// This does the actual work: updates the vtkPlane implicit function.
// This in turn causes the pipeline to update and clip the object.
class vtkIPWCallback : public vtkCommand
{
public:
  static vtkIPWCallback* New()
  {
    return new vtkIPWCallback;
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkImplicitPlaneWidget2* planeWidget =
        reinterpret_cast<vtkImplicitPlaneWidget2*>(caller);
    vtkImplicitPlaneRepresentation* rep =
        reinterpret_cast<vtkImplicitPlaneRepresentation*>(
            planeWidget->GetRepresentation());
    rep->GetPlane(this->plane);
  }

  vtkIPWCallback() = default;

  vtkPlane* plane{nullptr};
};

} // namespace

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(10.0);

  vtkNew<vtkXMLPolyDataReader> reader;

  // Setup a visualization pipeline.
  vtkNew<vtkPlane> plane;
  vtkNew<vtkClipPolyData> clipper;
  clipper->SetClipFunction(plane);
  clipper->InsideOutOn();
  if (argc < 2)
  {
    clipper->SetInputConnection(sphereSource->GetOutputPort());
  }
  else
  {
    reader->SetFileName(argv[1]);
    clipper->SetInputConnection(reader->GetOutputPort());
  }

  // Create a mapper and actor.
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(clipper->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkProperty> backFaces;
  backFaces->SetDiffuseColor(colors->GetColor3d("Gold").GetData());

  actor->SetBackfaceProperty(backFaces);

  // A renderer and render window.
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ImplicitPlaneWidget2");

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // An interactor.
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // The callback will do the work.
  vtkNew<vtkIPWCallback> myCallback;
  myCallback->plane = plane;

  vtkNew<vtkImplicitPlaneRepresentation> rep;
  rep->SetPlaceFactor(1.25); // This must be set prior to placing the widget.
  rep->PlaceWidget(actor->GetBounds());
  rep->SetNormal(plane->GetNormal());

  vtkNew<vtkImplicitPlaneWidget2> planeWidget;
  planeWidget->SetInteractor(renderWindowInteractor);
  planeWidget->SetRepresentation(rep);
  planeWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);

  renderer->GetActiveCamera()->Azimuth(-60);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(0.75);

  // Render and interact.
  renderWindowInteractor->Initialize();
  renderWindow->Render();
  planeWidget->On();

  // Begin mouse interaction.
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
