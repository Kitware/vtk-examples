#include <vtkDataSetAttributes.h>
#include <vtkDoubleArray.h>
#include <vtkForceDirectedLayoutStrategy.h>
#include <vtkGraphLayoutView.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
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

  // Create a fully connected graph
  g->AddEdge(v1, v2);
  g->AddEdge(v2, v3);
  g->AddEdge(v1, v3);

  // Create the edge weight array
  vtkNew<vtkDoubleArray> weights;
  weights->SetNumberOfComponents(1);
  weights->SetName("Weights");

  // Set the edge weights
  weights->InsertNextValue(1.0);
  weights->InsertNextValue(1.0);
  weights->InsertNextValue(2.0);

  // Add the edge weight array to the graph
  g->GetEdgeData()->AddArray(weights);

  std::cout << "Number of Weights: "
            << dynamic_cast<vtkDoubleArray*>(
                   g->GetEdgeData()->GetArray("Weights"))
                   ->GetNumberOfTuples()
            << std::endl;

  for (vtkIdType i = 0; i < weights->GetNumberOfTuples(); i++)
  {
    double w = weights->GetValue(i);
    std::cout << "Weight " << i << " : " << w << std::endl;
  }

  vtkNew<vtkForceDirectedLayoutStrategy> forceDirected;

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(g);
  // If we create a layout object directly, just set the pointer to this method.
  // graphLayoutView->SetLayoutStrategy(forceDirected);
  graphLayoutView->SetLayoutStrategyToForceDirected();
  graphLayoutView->AddRepresentationFromInput(g);
  graphLayoutView->SetEdgeLabelVisibility(true);
  graphLayoutView->SetEdgeLabelArrayName("Weights");
  graphLayoutView->ResetCamera();
  graphLayoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("Navy").GetData());
  graphLayoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("MidnightBlue").GetData());
  graphLayoutView->GetRenderWindow()->SetWindowName("EdgeWeights");
  graphLayoutView->Render();
  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
