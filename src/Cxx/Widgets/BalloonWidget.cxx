#include <vtkActor.h>
#include <vtkBalloonRepresentation.h>
#include <vtkBalloonWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Sphere.
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

  // Regular Polygon.
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

  // A renderer and render window.
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);
  renWin->SetWindowName("BalloonWidget");

  // An interactor.
  vtkNew<vtkRenderWindowInteractor> iRen;
  iRen->SetRenderWindow(renWin);

  // Create the widget.
  vtkNew<vtkBalloonRepresentation> balloonRep;
  balloonRep->SetBalloonLayoutToImageRight();

  vtkNew<vtkBalloonWidget> balloonWidget;
  balloonWidget->SetInteractor(iRen);
  balloonWidget->SetRepresentation(balloonRep);
  balloonWidget->AddBalloon(sphereActor, "This is a sphere", nullptr);
  balloonWidget->AddBalloon(regularPolygonActor, "This is a regular polygon",
                            nullptr);

  // Add the actors to the scene.
  ren->AddActor(sphereActor);
  ren->AddActor(regularPolygonActor);
  ren->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render
  renWin->Render();

  balloonWidget->EnabledOn();

  // Begin mouse interaction.
  iRen->Initialize();
  iRen->Start();

  return EXIT_SUCCESS;
}
