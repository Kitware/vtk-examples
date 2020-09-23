#include <vtkDataSetAttributes.h>
#include <vtkFloatArray.h>
#include <vtkGraphLayoutView.h>
#include <vtkGraphToGlyphs.h>
#include <vtkGraphWriter.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderedGraphRepresentation.h>
#include <vtkRenderer.h>

namespace {

class CustomRepresentation : public vtkRenderedGraphRepresentation
{
public:
  static CustomRepresentation* New();
  vtkTypeMacro(CustomRepresentation, vtkRenderedGraphRepresentation);

  void SetVertexSize(int vertexSize)
  {
    this->VertexGlyph->SetScreenSize(vertexSize);
    this->VertexGlyph->Modified();
  }
};
vtkStandardNewMacro(CustomRepresentation);
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMutableUndirectedGraph> g;

  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();

  g->AddEdge(v1, v2);
  g->AddEdge(v1, v2);

  // Specify coordinates so the graph is always the same for testing
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 0, 0);
  g->SetPoints(points);

  vtkNew<CustomRepresentation> representation;
  representation->SetInputData(g);
  representation->SetVertexSize(100);
  representation->SetGlyphType(vtkGraphToGlyphs::CIRCLE);

  vtkNew<vtkGraphLayoutView> layoutView;
  layoutView->AddRepresentation(representation);
  layoutView->SetLayoutStrategy("Pass Through");
  layoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("SaddleBrown").GetData());
  layoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("Wheat").GetData());
  layoutView->GetRenderWindow()->SetWindowName("VertexSize");
  layoutView->ResetCamera();
  layoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
