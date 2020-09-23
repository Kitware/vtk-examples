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
  randomGraphSource->SetSeed(0);
  randomGraphSource->Update();

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(randomGraphSource->GetOutput());
  graphLayoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("Navy").GetData());
  graphLayoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("MidnightBlue").GetData());
  graphLayoutView->GetRenderWindow()->SetWindowName("RandomGraphSource");
  graphLayoutView->ResetCamera();
  graphLayoutView->Render();
  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
