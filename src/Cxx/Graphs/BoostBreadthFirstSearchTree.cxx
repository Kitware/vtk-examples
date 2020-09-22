#include <vtkBoostBreadthFirstSearchTree.h>
#include <vtkDataSetAttributes.h>
#include <vtkDoubleArray.h>
#include <vtkGraphLayoutView.h>
#include <vtkIntArray.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNew.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTree.h>

int main(int, char*[])
{
  vtkNew<vtkMutableUndirectedGraph> g;

  // Create 5 vertices
  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();
  vtkIdType v3 = g->AddVertex();
  vtkIdType v4 = g->AddVertex();
  vtkIdType v5 = g->AddVertex();

  // Create a graph
  g->AddEdge(v1, v2);
  g->AddEdge(v1, v3);
  g->AddEdge(v2, v4);
  g->AddEdge(v4, v5);

  vtkNew<vtkBoostBreadthFirstSearchTree> bfsTree;
  bfsTree->SetOriginVertex(v5);
  bfsTree->SetInput(g);
  bfsTree->Update();

  {
    // Original graph
    vtkNew<vtkGraphLayoutView> graphLayoutView;
    graphLayoutView->AddRepresentationFromInput(g);
    graphLayoutView->SetLayoutStrategyToTree();
    graphLayoutView->ResetCamera();
    graphLayoutView->Render();
    graphLayoutView->GetInteractor()->Start();
  }

  {
    // BFS tree
    vtkNew<vtkGraphLayoutView> graphLayoutView;
    graphLayoutView->SetLayoutStrategyToTree();
    graphLayoutView->AddRepresentationFromInput(bfsTree->GetOutput());
    graphLayoutView->ResetCamera();
    graphLayoutView->Render();
    graphLayoutView->GetInteractor()->Start();
  }

  return EXIT_SUCCESS;
}
