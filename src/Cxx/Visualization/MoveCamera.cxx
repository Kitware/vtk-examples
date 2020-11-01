#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
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

  // Sphere 1
  vtkNew<vtkSphereSource> sphereSource1;
  sphereSource1->SetCenter(0.0, 0.0, 0.0);
  sphereSource1->SetRadius(4.0);
  sphereSource1->Update();

  vtkNew<vtkPolyDataMapper> mapper1;
  mapper1->SetInputConnection(sphereSource1->GetOutputPort());

  vtkNew<vtkActor> actor1;
  actor1->SetMapper(mapper1);
  actor1->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Sphere 2
  vtkNew<vtkSphereSource> sphereSource2;
  sphereSource2->SetCenter(10.0, 0.0, 0.0);
  sphereSource2->SetRadius(3.0);
  sphereSource2->Update();

  // Create a mapper
  vtkNew<vtkPolyDataMapper> mapper2;
  mapper2->SetInputConnection(sphereSource2->GetOutputPort());

  // Create an actor
  vtkNew<vtkActor> actor2;
  actor2->SetMapper(mapper2);
  actor2->GetProperty()->SetColor(colors->GetColor3d("Cornsilk").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("MoveCamera");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor1);
  renderer->AddActor(actor2);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Render an image (lights and cameras are created automatically)
  renderWindow->Render();

  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
      vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

  renderWindowInteractor->SetInteractorStyle(style);

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
