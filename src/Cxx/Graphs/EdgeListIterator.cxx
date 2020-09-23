#include <vtkEdgeListIterator.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNew.h>

int main(int, char*[])
{
  vtkNew<vtkMutableUndirectedGraph> g;

  // Create a graph
  vtkIdType v0 = g->AddVertex();
  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();

  g->AddEdge(v0, v1);
  g->AddEdge(v1, v2);
  g->AddEdge(v0, v2);

  vtkNew<vtkEdgeListIterator> edgeListIterator;
  g->GetEdges(edgeListIterator);

  while (edgeListIterator->HasNext())
  {
    vtkEdgeType edge = edgeListIterator->Next();
    std::cout << "Edge: " << edge.Id << " is from "
              << "Source: " << edge.Source << " to Target: " << edge.Target
              << std::endl;
  }

  return EXIT_SUCCESS;
}
