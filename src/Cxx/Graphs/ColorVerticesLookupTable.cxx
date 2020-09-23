#include <vtkCamera.h>
#include <vtkDataSetAttributes.h>
#include <vtkGraphLayoutView.h>
#include <vtkIntArray.h>
#include <vtkLookupTable.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkViewTheme.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a graph
  vtkNew<vtkMutableDirectedGraph> graph;

  vtkIdType v1 = graph->AddVertex();
  vtkIdType v2 = graph->AddVertex();
  vtkIdType v3 = graph->AddVertex();
  graph->AddEdge(v1, v2);
  graph->AddEdge(v2, v3);

  // Manually set the position of the vertices
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 0, 0);
  points->InsertNextPoint(2, 0, 0);

  graph->SetPoints(points);

  // Create the color array
  vtkNew<vtkIntArray> vertexColors;
  vertexColors->SetNumberOfComponents(1);
  vertexColors->SetName("Color");

  vtkNew<vtkLookupTable> lookupTable;
  lookupTable->SetNumberOfTableValues(3);
  lookupTable->SetTableValue(0, colors->GetColor4d("Red").GetData());
  lookupTable->SetTableValue(1, colors->GetColor4d("White").GetData());
  lookupTable->SetTableValue(2, colors->GetColor4d("Lime").GetData());
  lookupTable->Build();

  vertexColors->InsertNextValue(0);
  vertexColors->InsertNextValue(1);
  vertexColors->InsertNextValue(2);

  // Add the color array to the graph
  graph->GetVertexData()->AddArray(vertexColors);

  // Visualize
  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(graph);
  graphLayoutView->SetLayoutStrategyToPassThrough();
  graphLayoutView->SetVertexColorArrayName("Color");
  graphLayoutView->ColorVerticesOn();

  vtkNew<vtkViewTheme> theme;
  theme->SetPointLookupTable(lookupTable);

  graphLayoutView->ApplyViewTheme(theme);
  graphLayoutView->ResetCamera();
  graphLayoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("Navy").GetData());
  graphLayoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("MidnightBlue").GetData());
  graphLayoutView->GetRenderWindow()->SetWindowName("ColorVerticesLookupTable");
  graphLayoutView->GetInteractor()->Initialize();
  graphLayoutView->GetRenderer()->GetActiveCamera()->Zoom(0.8);
  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
