#include <vtkMutableDirectedGraph.h>
#include <vtkNew.h>
#include <vtkTree.h>
#include <vtkTreeBFSIterator.h>

int main(int argc, char* argv[])
{
  // create a tree
  vtkNew<vtkMutableDirectedGraph> g;

  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();
  vtkIdType v3 = g->AddVertex();

  g->AddEdge(v1, v2);
  g->AddEdge(v1, v3);

  vtkNew<vtkTree> tree;
  tree->CheckedShallowCopy(g);

  // setup the iterator
  vtkIdType root = 0;

  vtkNew<vtkTreeBFSIterator> bfsIterator;
  bfsIterator->SetStartVertex(root);
  bfsIterator->SetTree(tree);

  // traverse the tree in a breadth first fashion
  while (bfsIterator->HasNext())
  {
    vtkIdType nextVertex = bfsIterator->Next();
    std::cout << "Next vertex: " << nextVertex << std::endl;
  }

  return EXIT_SUCCESS;
}
