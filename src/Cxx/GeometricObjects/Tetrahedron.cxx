#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellType.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTetra.h>
#include <vtkUnstructuredGrid.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 0, 0);
  points->InsertNextPoint(1, 1, 0);
  points->InsertNextPoint(0, 1, 1);

  points->InsertNextPoint(2, 2, 2);
  points->InsertNextPoint(3, 2, 2);
  points->InsertNextPoint(3, 3, 2);
  points->InsertNextPoint(2, 3, 3);

  // Method 1
  vtkNew<vtkUnstructuredGrid> unstructuredGrid1;
  unstructuredGrid1->SetPoints(points);

  vtkIdType ptIds[] = {0, 1, 2, 3};
  unstructuredGrid1->InsertNextCell(VTK_TETRA, 4, ptIds);

  // Method 2
  vtkNew<vtkUnstructuredGrid> unstructuredGrid2;
  unstructuredGrid2->SetPoints(points);

  vtkNew<vtkTetra> tetra;

  tetra->GetPointIds()->SetId(0, 4);
  tetra->GetPointIds()->SetId(1, 5);
  tetra->GetPointIds()->SetId(2, 6);
  tetra->GetPointIds()->SetId(3, 7);

  vtkNew<vtkCellArray> cellArray;
  cellArray->InsertNextCell(tetra);
  unstructuredGrid2->SetCells(VTK_TETRA, cellArray);

  // Create a mapper and actor
  vtkNew<vtkDataSetMapper> mapper1;
  mapper1->SetInputData(unstructuredGrid1);

  vtkNew<vtkActor> actor1;
  actor1->SetMapper(mapper1);
  actor1->GetProperty()->SetColor(colors->GetColor3d("Cyan").GetData());

  // Create a mapper and actor
  vtkNew<vtkDataSetMapper> mapper2;
  mapper2->SetInputData(unstructuredGrid2);

  vtkNew<vtkActor> actor2;
  actor2->SetMapper(mapper2);
  actor2->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("Tetrahedron");
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor1);
  renderer->AddActor(actor2);
  renderer->SetBackground(colors->GetColor3d("DarkGreen").GetData());
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(-10);
  renderer->GetActiveCamera()->Elevation(-20);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
