#include <vtkSmartPointer.h>
#include <vtkGraphLayoutStrategy.h>
#include <vtkGraphLayoutView.h>
#include <vtkGraphWriter.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSimple2DLayoutStrategy.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNamedColors.h>

int main(int, char *[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMutableUndirectedGraph> g;

  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();

  g->AddEdge(v1, v2);
  g->AddEdge(v1, v2);

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(g);
  graphLayoutView->SetLayoutStrategy("Simple 2D");
  graphLayoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("SaddleBrown").GetData());
  graphLayoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("Wheat").GetData());
  graphLayoutView->GetRenderWindow()->SetWindowName("VisualizeGraph");
  graphLayoutView->ResetCamera();
  graphLayoutView->Render();

  dynamic_cast<vtkSimple2DLayoutStrategy*>(graphLayoutView->GetLayoutStrategy())
      ->SetRandomSeed(0);
  graphLayoutView->GetInteractor()->Start();
     
  return EXIT_SUCCESS;
}
