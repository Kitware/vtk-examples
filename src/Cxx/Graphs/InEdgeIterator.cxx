#include <vtkInEdgeIterator.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNew.h>

namespace {

void Undirected();

void Directed();

} // namespace

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

  // Find all incoming edges connected to a vertex
  vtkNew<vtkInEdgeIterator> it;
  g->GetInEdges(0, it);

  while (it->HasNext())
  {
    vtkInEdgeType edge = it->Next();
    std::cout << "Edge id: " << edge.Id << " Source: " << edge.Source
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

  g->AddEdge(v0, v1);
  g->AddEdge(v0, v2);

  {
    std::cout << "Finding edges connected to vertex 0" << std::endl;
    vtkNew<vtkInEdgeIterator> it;
    g->GetInEdges(0, it);

    while (it->HasNext())
    {
      vtkInEdgeType edge = it->Next();
      std::cout << "Edge id: " << edge.Id << " "
                << "Source: " << edge.Source << std::endl;
    }

    std::cout << "Nothing should be output, vertex 0 has no incoming edges!"
              << std::endl;
  }

  {
    std::cout << "Finding edges connected to vertex 1" << std::endl;
    vtkNew<vtkInEdgeIterator> it;
    g->GetInEdges(1, it);

    while (it->HasNext())
    {
      vtkInEdgeType edge = it->Next();
      std::cout << "Edge id: " << edge.Id << " "
                << "Source: " << edge.Source << std::endl;
    }
  }
}

} // namespace
