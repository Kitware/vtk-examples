#include <vtkGraphLayoutView.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMutableUndirectedGraph> g;

  // Create 3 vertices
  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();
  vtkIdType v3 = g->AddVertex();
  g->AddEdge(v1, v2);
  g->AddEdge(v1, v3);

  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(0.0, 1.0, 0.0);

  g->SetPoints(points);

  std::cout << "Number of output points: "
            << g->GetPoints()->GetNumberOfPoints() << std::endl;

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(g);
  graphLayoutView->SetLayoutStrategy("Pass Through");
  graphLayoutView->ResetCamera();
  graphLayoutView->GetRenderer()->GradientBackgroundOn();
  graphLayoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("RoyalBlue").GetData());
  graphLayoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("MidnightBlue").GetData());

  graphLayoutView->GetRenderWindow()->SetWindowName("GraphPoints");
  graphLayoutView->Render();
  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
