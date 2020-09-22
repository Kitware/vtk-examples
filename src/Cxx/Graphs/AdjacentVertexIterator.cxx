#include <vtkAdjacentVertexIterator.h>
#include <vtkDataSetAttributes.h>
#include <vtkGraphLayoutView.h>
#include <vtkLookupTable.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkUnsignedCharArray.h>
#include <vtkViewTheme.h>


// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

int main(int, char *[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMutableUndirectedGraph> g;

  //create 3 vertices
  vtkIdType v0 = g->AddVertex();
  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();
  vtkIdType v3 = g->AddVertex();

  //add edges
  g->AddEdge(v0, v1);
  g->AddEdge(v0, v2);
  g->AddEdge(v2, v3);

  // Create the color array
  vtkNew<vtkUnsignedCharArray> vertexColors;
  vertexColors->SetNumberOfComponents(3);
  vertexColors->SetName("color");

  //setup colors
   vertexColors->InsertNextTypedTuple(
        colors->GetColor3ub("Blue").GetData()); // unconnected vertices

  vertexColors->InsertNextTypedTuple(colors->GetColor3ub("Red").GetData()); // origin

  vtkNew<vtkAdjacentVertexIterator> iterator;
  g->GetAdjacentVertices(0, iterator);

  while(iterator->HasNext())
  {
    vtkIdType nextVertex = iterator->Next();
    std::cout << "Next vertex: " << nextVertex << std::endl;
    vertexColors->InsertNextTypedTuple(
        colors->GetColor3ub("Green").GetData()); // connected vertices
  }

  // Add the color array to the graph
  g->GetVertexData()->AddArray(vertexColors);

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(g);
  graphLayoutView->SetLayoutStrategyToTree();

  graphLayoutView->SetVertexColorArrayName("color");
  graphLayoutView->ColorVerticesOn();

  vtkNew<vtkViewTheme> theme;
  //theme->SetPointLookupTable(lookupTable);
  theme->ScalePointLookupTableOff();
  graphLayoutView->ApplyViewTheme(theme);


  graphLayoutView->ResetCamera();
  graphLayoutView->GetInteractor()->Initialize();
  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
