#include <vtkDataSetAttributes.h>
#include <vtkForceDirectedLayoutStrategy.h>
#include <vtkGraphLayoutView.h>
#include <vtkGraphToPolyData.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create the first graph
  vtkNew<vtkMutableUndirectedGraph> g0;
  {
    vtkIdType v1 = g0->AddVertex();
    vtkIdType v2 = g0->AddVertex();
    vtkIdType v3 = g0->AddVertex();

    g0->AddEdge(v1, v2);
    g0->AddEdge(v2, v3);
    g0->AddEdge(v1, v3);

    // Create points
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    // Add the coordinates of the points to the graph
    g0->SetPoints(points);
  }

  // Create the second graph
  vtkNew<vtkMutableUndirectedGraph> g1;

  vtkIdType v1 = g1->AddVertex();
  vtkIdType v2 = g1->AddVertex();

  g1->AddEdge(v1, v2);

  // Create points
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(1.0, 0.0, 0.0);

  // Add the coordinates of the points to the graph
  g1->SetPoints(points);

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("SideBySideGraphs");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  vtkNew<vtkForceDirectedLayoutStrategy> forceDirected;

  vtkNew<vtkGraphLayoutView> graphLayoutView0;
  // If we create a layout object directly, just set the pointer to this method.
  // graphLayoutView0->SetLayoutStrategy(forceDirected);
  graphLayoutView0->SetLayoutStrategyToForceDirected();
  graphLayoutView0->SetRenderWindow(renderWindow);
  graphLayoutView0->SetInteractor(renderWindowInteractor);
  graphLayoutView0->GetRenderer()->SetViewport(leftViewport);
  graphLayoutView0->AddRepresentationFromInput(g0);
  graphLayoutView0->GetRenderer()->SetBackground(
      colors->GetColor3d("Navy").GetData());
  graphLayoutView0->GetRenderer()->SetBackground2(
      colors->GetColor3d("MidnightBlue").GetData());
  graphLayoutView0->Render();
  graphLayoutView0->ResetCamera();

  vtkNew<vtkGraphLayoutView> graphLayoutView1;
  // If we create a layout object directly, just set the pointer to this method.
  // graphLayoutView1->SetLayoutStrategy(forceDirected);
  graphLayoutView1->SetLayoutStrategyToForceDirected();
  graphLayoutView1->SetRenderWindow(renderWindow);
  graphLayoutView1->SetInteractor(renderWindowInteractor);
  graphLayoutView1->GetRenderer()->SetViewport(rightViewport);
  graphLayoutView1->AddRepresentationFromInput(g1);
  graphLayoutView1->GetRenderer()->SetBackground(
      colors->GetColor3d("DarkGreen").GetData());
  graphLayoutView1->GetRenderer()->SetBackground2(
      colors->GetColor3d("ForestGreen").GetData());
  graphLayoutView1->Render();
  graphLayoutView1->ResetCamera();

  // graphLayoutView0->GetInteractor()->Start();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
