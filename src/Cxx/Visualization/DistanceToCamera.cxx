#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkDistanceToCamera.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  //---------------------------------------------------------------------------
  // Draw some arrows that maintain a fixed size during zooming.

  // Create a set of points.
  vtkNew<vtkPointSource> fixedPointSource;
  fixedPointSource->SetNumberOfPoints(2);

  // Calculate the distance to the camera of each point.
  vtkNew<vtkDistanceToCamera> distanceToCamera;
  distanceToCamera->SetInputConnection(fixedPointSource->GetOutputPort());
  distanceToCamera->SetScreenSize(100.0);

  // Glyph each point with an arrow.
  vtkNew<vtkArrowSource> arrow;
  vtkNew<vtkGlyph3D> fixedGlyph;
  fixedGlyph->SetInputConnection(distanceToCamera->GetOutputPort());
  fixedGlyph->SetSourceConnection(arrow->GetOutputPort());

  // Scale each point.
  fixedGlyph->SetScaleModeToScaleByScalar();
  fixedGlyph->SetInputArrayToProcess(
      0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera");

  // Create a mapper.
  vtkNew<vtkPolyDataMapper> fixedMapper;
  fixedMapper->SetInputConnection(fixedGlyph->GetOutputPort());
  fixedMapper->SetScalarVisibility(false);

  // Create an actor.
  vtkNew<vtkActor> fixedActor;
  fixedActor->SetMapper(fixedMapper);
  fixedActor->GetProperty()->SetColor(colors->GetColor3d("Cyan").GetData());

  //---------------------------------------------------------------------------
  // Draw some spheres that get bigger when zooming in.
  // Create a set of points.
  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(4);

  // Glyph each point with a sphere.
  vtkNew<vtkSphereSource> sphere;
  vtkNew<vtkGlyph3D> glyph;
  glyph->SetInputConnection(pointSource->GetOutputPort());
  glyph->SetSourceConnection(sphere->GetOutputPort());
  glyph->SetScaleFactor(0.1);

  // Create a mapper.
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(glyph->GetOutputPort());
  mapper->SetScalarVisibility(false);

  // Create an actor.
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

  //---------------------------------------------------------------------------

  // A renderer and render window.
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("DistanceToCamera");

  // Give DistanceToCamera a pointer to the renderer.
  distanceToCamera->SetRenderer(renderer);

  // Add the actors to the scene.
  renderer->AddActor(fixedActor);
  renderer->AddActor(actor);

  // An interactor.
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style);
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Render an image (lights and cameras are created automatically).
  renderWindow->Render();

  // Begin mouse interaction.
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
