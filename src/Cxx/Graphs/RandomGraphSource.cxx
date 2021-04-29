#include <vtkForceDirectedLayoutStrategy.h>
#include <vtkGraphLayoutView.h>
#include <vtkGraphWriter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRandomGraphSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRandomGraphSource> randomGraphSource;
  randomGraphSource->SetNumberOfVertices(5);
  randomGraphSource->SetNumberOfEdges(4);
  // This ensures repeatable results for testing. Turn this off for real use.
  randomGraphSource->SetSeed(123);
  randomGraphSource->Update();

  vtkNew<vtkForceDirectedLayoutStrategy> forceDirected;

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(randomGraphSource->GetOutput());
  // If we create a layout object directly, just set the pointer to this method.
  // graphLayoutView->SetLayoutStrategy(forceDirected);
  graphLayoutView->SetLayoutStrategyToForceDirected();
  graphLayoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("Navy").GetData());
  graphLayoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("MidnightBlue").GetData());
  graphLayoutView->GetRenderWindow()->SetWindowName("RandomGraphSource");
  graphLayoutView->Render();
  graphLayoutView->ResetCamera();
  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
