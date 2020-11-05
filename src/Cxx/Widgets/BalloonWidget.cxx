#include <vtkActor.h>
#include <vtkBalloonRepresentation.h>
#include <vtkBalloonWidget.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkSphereSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(-4.0, 0.0, 0.0);
  sphereSource->SetRadius(4.0);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // Regular Polygon
  vtkNew<vtkRegularPolygonSource> regularPolygonSource;
  regularPolygonSource->SetCenter(4.0, 0.0, 0.0);
  regularPolygonSource->SetRadius(4.0);
  regularPolygonSource->Update();

  vtkNew<vtkPolyDataMapper> regularPolygonMapper;
  regularPolygonMapper->SetInputConnection(
      regularPolygonSource->GetOutputPort());

  vtkNew<vtkActor> regularPolygonActor;
  regularPolygonActor->SetMapper(regularPolygonMapper);
  regularPolygonActor->GetProperty()->SetColor(
      colors->GetColor3d("Cornsilk").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("BalloonWidget");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Create the widget
  vtkNew<vtkBalloonRepresentation> balloonRep;
  balloonRep->SetBalloonLayoutToImageRight();

  vtkNew<vtkBalloonWidget> balloonWidget;
  balloonWidget->SetInteractor(renderWindowInteractor);
  balloonWidget->SetRepresentation(balloonRep);
  balloonWidget->AddBalloon(sphereActor, "This is a sphere", NULL);
  balloonWidget->AddBalloon(regularPolygonActor, "This is a regular polygon",
                            NULL);

  // Add the actors to the scene
  renderer->AddActor(sphereActor);
  renderer->AddActor(regularPolygonActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render
  renderWindow->Render();
  balloonWidget->EnabledOn();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
