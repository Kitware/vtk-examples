#include <vtkCamera.h>
#include <vtkDataSetAttributes.h>
#include <vtkGraphLayout.h>
#include <vtkGraphLayoutStrategy.h>
#include <vtkGraphLayoutView.h>
#include <vtkIntArray.h>
#include <vtkLookupTable.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkViewTheme.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMutableDirectedGraph> graph;

  // Create a graph
  vtkIdType v1 = graph->AddVertex();
  vtkIdType v2 = graph->AddVertex();
  vtkIdType v3 = graph->AddVertex();
  graph->AddEdge(v1, v2);
  graph->AddEdge(v2, v3);

  // Create the color array
  vtkNew<vtkIntArray> edgeColors;
  edgeColors->SetNumberOfComponents(1);
  edgeColors->SetName("Color");

  vtkNew<vtkLookupTable> lookupTable;
  lookupTable->SetNumberOfTableValues(2);
  lookupTable->SetTableValue(0, colors->GetColor4d("Red").GetData());
  lookupTable->SetTableValue(1, colors->GetColor4d("Lime").GetData());
  lookupTable->Build();

  edgeColors->InsertNextValue(0);
  edgeColors->InsertNextValue(1);

  // Add the color array to the graph
  graph->GetEdgeData()->AddArray(edgeColors);

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(graph);
  // Needs VTK::InfovisBoostGraphAlgorithms
  // graphLayoutView->SetLayoutStrategyToTree();
  graphLayoutView->SetLayoutStrategy("Simple 2D");

  graphLayoutView->SetEdgeColorArrayName("Color");
  graphLayoutView->ColorEdgesOn();

  vtkNew<vtkViewTheme> theme;
  theme->SetCellLookupTable(lookupTable);

  graphLayoutView->ApplyViewTheme(theme);
  graphLayoutView->ResetCamera();
  // graphLayoutView->GetRenderer()->SetBackground(
  //    colors->GetColor3d("Navy").GetData());
  // graphLayoutView->GetRenderer()->SetBackground2(
  //    colors->GetColor3d("MidnightBlue").GetData());
  graphLayoutView->GetRenderer()->GetActiveCamera()->Zoom(0.8);
  graphLayoutView->GetRenderWindow()->SetWindowName("ColorEdges");
  graphLayoutView->GetInteractor()->Initialize();
  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
