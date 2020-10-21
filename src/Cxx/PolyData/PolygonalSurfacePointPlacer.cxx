#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkContourWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkPolyDataCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace {

class MyCallback : public vtkCommand
{
public:
  static MyCallback* New()
  {
    return new MyCallback;
  }
  MyCallback()
  {
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkContourWidget* contourWidget =
        reinterpret_cast<vtkContourWidget*>(caller);
    vtkContourRepresentation* rep = static_cast<vtkContourRepresentation*>(
        contourWidget->GetRepresentation());
    std::cout << "There are " << rep->GetNumberOfNodes() << " nodes."
              << std::endl;
  }

  void SetSphereSource(vtkSmartPointer<vtkSphereSource> sphere)
  {
    this->SphereSource = sphere;
  }

private:
  vtkSmartPointer<vtkSphereSource> SphereSource;
};

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(5);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create the RenderWindow, Renderer
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("PolygonalSurfacePointPlacer");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("CadetBlue").GetData());

  vtkNew<vtkContourWidget> contourWidget;
  contourWidget->SetInteractor(interactor);

  vtkNew<MyCallback> callback;
  callback->SetSphereSource(sphereSource);
  contourWidget->AddObserver(vtkCommand::InteractionEvent, callback);

  vtkOrientedGlyphContourRepresentation* rep =
      dynamic_cast<vtkOrientedGlyphContourRepresentation*>(
          contourWidget->GetRepresentation());

  vtkNew<vtkPolygonalSurfacePointPlacer> pointPlacer;
  pointPlacer->AddProp(actor);
  pointPlacer->GetPolys()->AddItem(sphereSource->GetOutput());

  rep->GetLinesProperty()->SetColor(colors->GetColor3d("Crimson").GetData());
  rep->GetLinesProperty()->SetLineWidth(3.0);
  rep->SetPointPlacer(pointPlacer);

  contourWidget->EnabledOn();
  renderer->ResetCamera();
  renderWindow->Render();
  interactor->Initialize();

  interactor->Start();

  return EXIT_SUCCESS;
}
