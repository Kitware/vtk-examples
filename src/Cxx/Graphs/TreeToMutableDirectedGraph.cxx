#include <vtkDirectedGraph.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkTree.h>

int main(int, char*[])
{

  // create a graph
  vtkNew<vtkMutableDirectedGraph> mdg;

  // add 4 vertices to the graph
  vtkIdType v1 = mdg->AddVertex();
  vtkIdType v2 = mdg->AddVertex();
  vtkIdType v3 = mdg->AddVertex();
  vtkIdType v4 = mdg->AddVertex();

  // add 3 edges to the graph
  mdg->AddEdge(v1, v2);
  mdg->AddEdge(v1, v3);
  mdg->AddEdge(v1, v4);

  // create 4 points - one for each vertex
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(0.0, 1.0, 0.0);
  points->InsertNextPoint(0.0, 0.0, 2.0);

  // add the coordinates of the points to the graph
  mdg->SetPoints(points);

  std::cout << "MDG: " << std::endl;
  std::cout << "Type: " << mdg->GetClassName() << std::endl;
  std::cout << "Vertices: " << mdg->GetNumberOfVertices() << std::endl;
  std::cout << "Edges: " << mdg->GetNumberOfEdges() << std::endl;

  vtkNew<vtkTree> tree;
  tree->CheckedShallowCopy(mdg);

  std::cout << "Tree: " << std::endl;
  std::cout << "Type: " << tree->GetClassName() << std::endl;
  std::cout << "Vertices: " << tree->GetNumberOfVertices() << std::endl;
  std::cout << "Edges: " << tree->GetNumberOfEdges() << std::endl;

  vtkNew<vtkMutableDirectedGraph> mdg2;
  if (!mdg2->CheckedShallowCopy(tree))
  {
    std::cerr << "Could not convert!" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "MDG2: " << std::endl;
  std::cout << "Type: " << mdg2->GetClassName() << std::endl;
  std::cout << "Vertices: " << mdg2->GetNumberOfVertices() << std::endl;
  std::cout << "Edges: " << mdg2->GetNumberOfEdges() << std::endl;

  return EXIT_SUCCESS;
}
