#include <vtkGraphLayoutView.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTree.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMutableDirectedGraph> graph;

  vtkIdType v1 = graph->AddVertex();
  vtkIdType v2 = graph->AddChild(v1);
  graph->AddChild(v1);
  graph->AddChild(v2);

  /*
  //equivalent to:

  vtkIdType V1 = g->AddVertex();
  vtkIdType V2 = g->AddVertex();
  vtkIdType V3 = g->AddVertex();
  vtkIdType V4 = g->AddVertex();

  g->AddEdge ( V1, V2 );
  g->AddEdge ( V1, V3 );
  g->AddEdge ( V2, V4 );
  */

  vtkNew<vtkTree> tree;
  bool success = tree->CheckedShallowCopy(graph);
  std::cout << "Success? " << success << std::endl;

  vtkNew<vtkGraphLayoutView> treeLayoutView;
  treeLayoutView->AddRepresentationFromInput(tree);
  treeLayoutView->SetLayoutStrategyToTree();
  treeLayoutView->ResetCamera();
  treeLayoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("Navy").GetData());
  treeLayoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("MidnightBlue").GetData());
  treeLayoutView->GetRenderWindow()->SetWindowName("ConstructTree");
  treeLayoutView->Render();
  treeLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
