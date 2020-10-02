#include <vtkGraphLayoutView.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkMutableGraphHelper.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNew.h>
#include <vtkRenderWindowInteractor.h>

int main(int, char*[])
{
  vtkNew<vtkMutableUndirectedGraph> g;

  vtkNew<vtkMutableGraphHelper> graphHelper;
  graphHelper->SetGraph(g);
  vtkIdType v0 = graphHelper->AddVertex();
  vtkIdType v1 = graphHelper->AddVertex();

  graphHelper->AddEdge(v0, v1);

  // Can also do this:
  graphHelper->RemoveEdge(0);

  vtkNew<vtkGraphLayoutView> treeLayoutView;
  treeLayoutView->AddRepresentationFromInput(graphHelper->GetGraph());
  treeLayoutView->SetLayoutStrategyToTree();
  treeLayoutView->ResetCamera();
  treeLayoutView->Render();
  treeLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
