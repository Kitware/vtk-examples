#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCompassRepresentation.h>
#include <vtkCompassWidget.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkArrowSource> arrowSource;

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(arrowSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("PeachPuff").GetData());

  // a renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  // an interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Create the widget and its representation
  vtkNew<vtkCompassRepresentation> compassRepresentation;

  vtkNew<vtkCompassWidget> compassWidget;
  compassWidget->SetInteractor(renderWindowInteractor);
  compassWidget->SetRepresentation(compassRepresentation);

  // add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("CompassWidget");

  renderWindow->Render();
  compassWidget->EnabledOn();

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style);

  // begin interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
