#include <vtkDataSetAttributes.h>
#include <vtkFloatArray.h>
#include <vtkForceDirectedLayoutStrategy.h>
#include <vtkGraphLayoutView.h>
#include <vtkGraphToGlyphs.h>
#include <vtkGraphWriter.h>
#include <vtkLookupTable.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderedGraphRepresentation.h>
#include <vtkRenderer.h>
#include <vtkViewTheme.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMutableUndirectedGraph> g;

  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();

  g->AddEdge(v1, v2);
  g->AddEdge(v1, v2);

  vtkNew<vtkFloatArray> scales;
  scales->SetNumberOfComponents(1);
  scales->SetName("Scales");
  scales->InsertNextValue(2.0);
  scales->InsertNextValue(5.0);

  // Add the scale array to the graph
  g->GetVertexData()->AddArray(scales);

  // Create the color array
  vtkNew<vtkIntArray> vertexColors;
  vertexColors->SetNumberOfComponents(1);
  vertexColors->SetName("Color");

  vtkNew<vtkLookupTable> lookupTable;
  lookupTable->SetNumberOfTableValues(2);
  lookupTable->SetTableValue(0, colors->GetColor4d("Yellow").GetData());
  lookupTable->SetTableValue(1, colors->GetColor4d("Lime").GetData());
  lookupTable->Build();

  vertexColors->InsertNextValue(0);
  vertexColors->InsertNextValue(1);

  // Add the color array to the graph
  g->GetVertexData()->AddArray(vertexColors);

  // Visualize

  vtkNew<vtkViewTheme> theme;
  theme->SetPointLookupTable(lookupTable);

  vtkNew<vtkForceDirectedLayoutStrategy> forceDirected;

  vtkNew<vtkGraphLayoutView> layoutView;
  layoutView->AddRepresentationFromInput(g);
  // If we create a layout object directly, just set the pointer to this method.
  // graphLayoutView->SetLayoutStrategy(forceDirected);
  layoutView->SetLayoutStrategyToForceDirected();
  layoutView->ApplyViewTheme(theme);
  layoutView->ScaledGlyphsOn();
  layoutView->SetScalingArrayName("Scales");
  layoutView->SetVertexColorArrayName("Color");
  layoutView->ColorVerticesOn();
  dynamic_cast<vtkRenderedGraphRepresentation*>(layoutView->GetRepresentation())
      ->SetGlyphType(vtkGraphToGlyphs::CIRCLE);
  layoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("Navy").GetData());
  layoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("MidnightBlue").GetData());
  layoutView->GetRenderWindow()->SetWindowName("ScaleVertices");
  layoutView->Render();
  layoutView->ResetCamera();
  layoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
