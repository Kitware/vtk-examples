#include <vtkActor.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkGraphLayoutView.h>
#include <vtkGraphToPolyData.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTree.h>

/*
   O v0
  /|\
 / |5\
v1-v2-v3
  1  1

  Shortest path between v0 and v2 should be 5
*/

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMutableDirectedGraph> graph;
  vtkIdType v0 = graph->AddVertex();
  vtkIdType v1 = graph->AddVertex();
  vtkIdType v2 = graph->AddVertex();
  vtkIdType v3 = graph->AddVertex();

  graph->AddEdge(v0, v1);
  graph->AddEdge(v0, v2);
  graph->AddEdge(v0, v3);
  graph->AddEdge(v1, v2);
  graph->AddEdge(v2, v3);

  // Associate physical locations with the vertices
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(-1.0, -5.0, 0.0);
  points->InsertNextPoint(0.0, -5.0, 0.0);
  points->InsertNextPoint(1.0, -5.0, 0.0);

  graph->SetPoints(points);

  // Convert the graph to a polydata
  vtkNew<vtkGraphToPolyData> graphToPolyData;
  graphToPolyData->SetInputData(graph);
  graphToPolyData->Update();

  vtkNew<vtkDijkstraGraphGeodesicPath> dijkstra;
  dijkstra->SetInputConnection(graphToPolyData->GetOutputPort());
  dijkstra->SetStartVertex(0);
  dijkstra->SetEndVertex(2);
  dijkstra->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> pathMapper;
  pathMapper->SetInputConnection(dijkstra->GetOutputPort());

  vtkNew<vtkActor> pathActor;
  pathActor->SetMapper(pathMapper);
  pathActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
  pathActor->GetProperty()->SetLineWidth(4);

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(graphToPolyData->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  pathActor->GetProperty()->SetColor(
      colors->GetColor3d("NavajoWhite").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->AddActor(pathActor);
  renderer->SetBackground(colors->GetColor3d("DarkGreen").GetData());
  renderer->SetBackground2(colors->GetColor3d("ForestGreen").GetData());
  renderer->GradientBackgroundOn();
  renderWindow->SetWindowName("ShortestPath");

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
