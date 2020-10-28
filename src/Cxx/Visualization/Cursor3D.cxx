#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCursor3D.h>
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
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5.0);
  sphereSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->SetWindowName("Cursor3D");

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  vtkNew<vtkCursor3D> cursor;
  cursor->SetModelBounds(-10, 10, -10, 10, -10, 10);
  cursor->AllOn();
  cursor->OutlineOff();
  cursor->Update();

  vtkNew<vtkPolyDataMapper> cursorMapper;
  cursorMapper->SetInputConnection(cursor->GetOutputPort());
  vtkNew<vtkActor> cursorActor;
  cursorActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
  cursorActor->SetMapper(cursorMapper);
  renderer->AddActor(cursorActor);
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCamera();

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
