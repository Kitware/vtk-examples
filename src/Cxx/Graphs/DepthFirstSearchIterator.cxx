#include <vtkBoostBreadthFirstSearch.h>
#include <vtkBoostPrimMinimumSpanningTree.h>
#include <vtkDataSetAttributes.h>
#include <vtkDoubleArray.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNew.h>
#include <vtkTree.h>
#include <vtkTreeDFSIterator.h>

int main(int, char*[])
{
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

  // Add the edge weight array to the graph
  g->GetEdgeData()->AddArray(weights);

  // Output original graph info
  std::cout << "Original Graph" << std::endl << "----------" << std::endl;
  std::cout << "Number of vertices: " << g->GetNumberOfVertices() << std::endl;
  std::cout << "Number of edges: " << g->GetNumberOfEdges() << std::endl;

  // Setup the minimum spanning tree filter
  vtkNew<vtkBoostPrimMinimumSpanningTree> minimumSpanningTreeFilter;
  minimumSpanningTreeFilter->SetOriginVertex(v1);
  minimumSpanningTreeFilter->SetInput(g);
  minimumSpanningTreeFilter->SetEdgeWeightArrayName("Weights");

  // Compute the minimum spanning tree
  minimumSpanningTreeFilter->Update();

  // Get the output tree
  vtkNew<vtkTree> minimumSpanningTree;
  minimumSpanningTree->ShallowCopy(minimumSpanningTreeFilter->GetOutput());

  // Output information about the minimum spanning tree
  std::cout << std::endl;
  std::cout << "Minimum spanning tree" << std::endl
            << "----------" << std::endl;
  std::cout << "Number of vertices: "
            << minimumSpanningTree->GetNumberOfVertices() << std::endl;
  std::cout << "Number of edges: " << minimumSpanningTree->GetNumberOfEdges()
            << std::endl;

  vtkIdType root = minimumSpanningTree->GetRoot();
  std::cout << "Root: " << root << std::endl;

  vtkNew<vtkTreeDFSIterator> dFS;
  dFS->SetStartVertex(root);
  dFS->SetTree(minimumSpanningTree);

  // Traverse the tree in a depth first fashion
  while (dFS->HasNext())
  {
    vtkIdType NextVertex = dFS->Next();
    std::cout << "Next vertex: " << NextVertex
              << " level: " << minimumSpanningTree->GetLevel(NextVertex)
              << std::endl;
  }

  return EXIT_SUCCESS;
}
