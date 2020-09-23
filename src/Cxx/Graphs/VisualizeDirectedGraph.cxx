#include <vtkActor.h>
#include <vtkGlyph3D.h>
#include <vtkGlyphSource2D.h>
#include <vtkGraphLayout.h>
#include <vtkGraphLayoutView.h>
#include <vtkGraphToPolyData.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSimple2DLayoutStrategy.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMutableDirectedGraph> g;

  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();
  vtkIdType v3 = g->AddVertex();

  g->AddEdge(v1, v2);
  g->AddEdge(v2, v3);
  g->AddEdge(v3, v1);

  // Do layout manually before handing graph to the view.
  // This allows us to know the positions of edge arrows.
  vtkNew<vtkGraphLayoutView> graphLayoutView;

  vtkNew<vtkGraphLayout> layout;
  vtkNew<vtkSimple2DLayoutStrategy> strategy;
  layout->SetInputData(g);
  layout->SetLayoutStrategy(strategy);

  // Tell the view to use the vertex layout we provide
  graphLayoutView->SetLayoutStrategyToPassThrough();
  // The arrows will be positioned on a straight line between two
  // vertices so tell the view not to draw arcs for parallel edges
  graphLayoutView->SetEdgeLayoutStrategyToPassThrough();

  // Add the graph to the view. This will render vertices and edges,
  // but not edge arrows.
  graphLayoutView->AddRepresentationFromInputConnection(
      layout->GetOutputPort());

  // Manually create an actor containing the glyphed arrows.
  vtkNew<vtkGraphToPolyData> graphToPoly;
  graphToPoly->SetInputConnection(layout->GetOutputPort());
  graphToPoly->EdgeGlyphOutputOn();

  // Set the position (0: edge start, 1: edge end) where
  // the edge arrows should go.
  graphToPoly->SetEdgeGlyphPosition(0.98);

  // Make a simple edge arrow for glyphing.
  vtkNew<vtkGlyphSource2D> arrowSource;
  arrowSource->SetGlyphTypeToEdgeArrow();
  arrowSource->SetScale(0.1);
  arrowSource->Update();

  // Use Glyph3D to repeat the glyph on all edges.
  vtkNew<vtkGlyph3D> arrowGlyph;
  arrowGlyph->SetInputConnection(0, graphToPoly->GetOutputPort(1));
  arrowGlyph->SetInputConnection(1, arrowSource->GetOutputPort());

  // Add the edge arrow actor to the view.
  vtkNew<vtkPolyDataMapper> arrowMapper;
  arrowMapper->SetInputConnection(arrowGlyph->GetOutputPort());
  vtkNew<vtkActor> arrowActor;
  arrowActor->SetMapper(arrowMapper);
  graphLayoutView->GetRenderer()->AddActor(arrowActor);

  graphLayoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("SaddleBrown").GetData());
  graphLayoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("Wheat").GetData());
  graphLayoutView->GetRenderWindow()->SetWindowName("VisualizeDirectedGraph");
  graphLayoutView->ResetCamera();
  graphLayoutView->Render();
  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
