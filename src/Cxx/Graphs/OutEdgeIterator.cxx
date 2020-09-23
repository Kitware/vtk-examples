#include <vtkMutableUndirectedGraph.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNew.h>
#include <vtkOutEdgeIterator.h>

namespace {

void Undirected();

void Directed();

}

int main(int, char*[])
{
  Undirected();
  Directed();

  return EXIT_SUCCESS;
}

namespace {
void Undirected()
{
  vtkNew<vtkMutableUndirectedGraph> g;

  // Create a graph
  vtkIdType v0 = g->AddVertex();
  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();

  g->AddEdge(v0, v1);
  g->AddEdge(v1, v2);
  g->AddEdge(v0, v2);

  // Find all outgoing edges connected to a vertex
  vtkNew<vtkOutEdgeIterator> it;
  g->GetOutEdges(0, it); // Get the edges connected to vertex 0

  while (it->HasNext())
  {
    vtkOutEdgeType edge = it->Next();
    std::cout << "Edge id: " << edge.Id << " Target: " << edge.Target
              << std::endl;
  }
}

void Directed()
{
  vtkNew<vtkMutableDirectedGraph> g;

  // Create a graph
  vtkIdType v0 = g->AddVertex();
  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();

  g->AddEdge(v1, v0);
  g->AddEdge(v2, v0);

  {
    std::cout << "Finding edges connected to vertex 0" << std::endl;
    vtkNew<vtkOutEdgeIterator> it;
    g->GetOutEdges(0, it);

    while (it->HasNext())
    {
      vtkOutEdgeType edge = it->Next();
      std::cout << "Edge id: " << edge.Id << " Target: " << edge.Target
                << std::endl;
    }

    std::cout << "Nothing should be output, vertex 0 has no outgoing edges!"
              << std::endl;
  }

  {
    std::cout << "Finding edges connected to vertex 1" << std::endl;
    vtkNew<vtkOutEdgeIterator> it;
    g->GetOutEdges(1, it);

    while (it->HasNext())
    {
      vtkOutEdgeType edge = it->Next();
      std::cout << "Edge id: " << edge.Id << " Target: " << edge.Target
                << std::endl;
    }
  }
}

}
