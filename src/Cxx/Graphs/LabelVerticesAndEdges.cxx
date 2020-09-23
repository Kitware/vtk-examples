#include <vtkCircularLayoutStrategy.h>
#include <vtkDataSetAttributes.h>
#include <vtkDoubleArray.h>
#include <vtkGraphLayoutView.h>
#include <vtkIntArray.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderedGraphRepresentation.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMutableUndirectedGraph> g;

  // Create 3 vertices
  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();
  vtkIdType v3 = g->AddVertex();

  // Create a fully connected graph
  g->AddEdge(v1, v2);
  g->AddEdge(v2, v3);
  g->AddEdge(v1, v3);

  // Create the edge weight array
  vtkNew<vtkDoubleArray> weights;
  weights->SetNumberOfComponents(1);
  weights->SetName("Weights");

  // Set the edge weights
  weights->InsertNextValue(1.0);
  weights->InsertNextValue(1.0);
  weights->InsertNextValue(2.0);

  // Create an array for the vertex labels
  vtkNew<vtkIntArray> vertexIDs;
  vertexIDs->SetNumberOfComponents(1);
  vertexIDs->SetName("VertexIDs");

  // Set the vertex labels
  vertexIDs->InsertNextValue(0);
  vertexIDs->InsertNextValue(1);
  vertexIDs->InsertNextValue(2);

  // Add the edge weight array to the graph
  g->GetEdgeData()->AddArray(weights);
  g->GetVertexData()->AddArray(vertexIDs);

  vtkNew<vtkCircularLayoutStrategy> circularLayoutStrategy;

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(g);

  graphLayoutView->SetLayoutStrategy(circularLayoutStrategy);
  graphLayoutView->SetVertexLabelVisibility(true);
  graphLayoutView->SetEdgeLabelVisibility(true);
  graphLayoutView->SetEdgeLabelArrayName("Weights");     // default is "labels"
  graphLayoutView->SetVertexLabelArrayName("VertexIDs"); // default is "labels"
  dynamic_cast<vtkRenderedGraphRepresentation*>(
      graphLayoutView->GetRepresentation())
      ->GetVertexLabelTextProperty()
      ->SetColor(colors->GetColor3d("Yellow").GetData());
  dynamic_cast<vtkRenderedGraphRepresentation*>(
      graphLayoutView->GetRepresentation())
      ->GetEdgeLabelTextProperty()
      ->SetColor(colors->GetColor3d("Lime").GetData());
  graphLayoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("Navy").GetData());
  graphLayoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("MidnightBlue").GetData());
  graphLayoutView->GetRenderWindow()->SetWindowName("LabelVerticesAndEdges");
  graphLayoutView->ResetCamera();
  graphLayoutView->Render();
  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
