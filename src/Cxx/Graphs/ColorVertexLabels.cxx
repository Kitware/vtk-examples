#include <vtkCamera.h>
#include <vtkDataSetAttributes.h>
#include <vtkGraphLayoutView.h>
#include <vtkIntArray.h>
#include <vtkMutableDirectedGraph.h>
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

  // Create a graph
  vtkNew<vtkMutableDirectedGraph> graph;

  vtkIdType v1 = graph->AddVertex();
  vtkIdType v2 = graph->AddVertex();
  graph->AddEdge(v1, v2);

  // Create an array for the vertex labels
  vtkNew<vtkIntArray> vertexIDs;
  vertexIDs->SetNumberOfComponents(1);
  vertexIDs->SetName("VertexIDs");

  // Set the vertex labels
  vertexIDs->InsertNextValue(0);
  vertexIDs->InsertNextValue(1);

  // Add the array to the graph
  graph->GetVertexData()->AddArray(vertexIDs);

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(graph);
  graphLayoutView->SetVertexLabelVisibility(true);
  dynamic_cast<vtkRenderedGraphRepresentation*>(
      graphLayoutView->GetRepresentation())
      ->GetVertexLabelTextProperty()
      ->SetColor(colors->GetColor3d("Red").GetData());
  graphLayoutView->SetLayoutStrategyToSimple2D();
  graphLayoutView->SetVertexLabelArrayName("VertexIDs");
  graphLayoutView->ResetCamera();
  graphLayoutView->GetRenderer()->GetActiveCamera()->Zoom(0.8);
  graphLayoutView->GetRenderWindow()->SetWindowName("ColorVertexLabels");
  graphLayoutView->Render();
  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
