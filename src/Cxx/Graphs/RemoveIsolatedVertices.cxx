#include <vtkGraphAlgorithm.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkRemoveIsolatedVertices.h>
#include <vtkUndirectedGraph.h>

int main(int, char*[])
{
  vtkNew<vtkMutableUndirectedGraph> g;

  // Create 3 vertices
  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();
  g->AddVertex();

  g->AddEdge(v1, v2);

  std::cout << "Graph has " << g->GetNumberOfVertices() << " vertices and "
            << g->GetNumberOfEdges() << " edge before." << std::endl;

  vtkNew<vtkRemoveIsolatedVertices> filter;
  filter->SetInputData(g);
  filter->Update();

  std::cout << "Graph has " << filter->GetOutput()->GetNumberOfVertices()
            << " vertices and " << filter->GetOutput()->GetNumberOfEdges()
            << " edge after." << std::endl;

  return EXIT_SUCCESS;
}
