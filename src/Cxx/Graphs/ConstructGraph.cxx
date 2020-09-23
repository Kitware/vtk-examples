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
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMutableUndirectedGraph> g;

  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();

  g->AddEdge(v1, v2);

  std::cout << "Number of vertices: " << g->GetNumberOfVertices() << std::endl;
  std::cout << "Number of edges: " << g->GetNumberOfEdges() << std::endl;

  g->AddEdge(v1, v2);

  std::cout << "Number of vertices: " << g->GetNumberOfVertices() << std::endl;
  std::cout << "Number of edges: " << g->GetNumberOfEdges() << std::endl;

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(g);
  graphLayoutView->ResetCamera();
  graphLayoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("Navy").GetData());
  graphLayoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("MidnightBlue").GetData());
  graphLayoutView->GetRenderWindow()->SetWindowName("ConstructGraph");
  graphLayoutView->Render();
  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
