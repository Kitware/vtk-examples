#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkBrownianPoints.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;

  // Generate random vectors
  vtkMath::RandomSeed(5070); // for testing
  vtkNew<vtkBrownianPoints> brownianPoints;
  brownianPoints->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkArrowSource> arrowSource;

  vtkNew<vtkGlyph3D> glyph3D;
  glyph3D->SetSourceConnection(arrowSource->GetOutputPort());
  glyph3D->SetInputConnection(brownianPoints->GetOutputPort());
  glyph3D->SetScaleFactor(0.3);

  // Create a mapper and actor for sphere
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->GetProperty()->EdgeVisibilityOn();
  actor->GetProperty()->SetInterpolationToFlat();
  actor->SetMapper(mapper);

  // Create a mapper and actor for glyphs
  vtkNew<vtkPolyDataMapper> glyphMapper;
  glyphMapper->SetInputConnection(glyph3D->GetOutputPort());

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> glyphActor;
  glyphActor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
  glyphActor->SetMapper(glyphMapper);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->AddActor(glyphActor);

  // Create a nice view
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.4);
  renderer->ResetCameraClippingRange();

  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("BrownianPoints");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
