#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDijkstraGraphGeodesicPath.h>
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
  sphereSource->Update();

  vtkNew<vtkDijkstraGraphGeodesicPath> dijkstra;
  dijkstra->SetInputConnection(sphereSource->GetOutputPort());
  dijkstra->SetStartVertex(0);
  // dijkstra->SetEndVertex(10);
  dijkstra->SetEndVertex(7);
  dijkstra->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> pathMapper;
  pathMapper->SetInputConnection(dijkstra->GetOutputPort());

  vtkNew<vtkActor> pathActor;
  pathActor->SetMapper(pathMapper);
  pathActor->GetProperty()->SetColor(colors->GetColor3d("HotPink").GetData());
  pathActor->GetProperty()->SetLineWidth(4);

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
  renderWindow->SetWindowName("DijkstraGraphGeodesicPath");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->AddActor(pathActor);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(2.9, -1.6, 0.3);
  camera->SetFocalPoint(0, 0, 0);
  camera->SetViewUp(0, 0, 1);
  camera->SetDistance(3);
  camera->Zoom(1.5);

  // Render and interact
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
