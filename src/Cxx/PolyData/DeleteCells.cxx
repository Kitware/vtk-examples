#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkNew.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNamedColors.h>

int main(int, char*[])
{
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 0, 0);
  points->InsertNextPoint(1, 1, 0);
  points->InsertNextPoint(0, 1, 0);

  vtkNew<vtkLine> line0;
  line0->GetPointIds()->SetId(0, 0);
  line0->GetPointIds()->SetId(1, 1);

  vtkNew<vtkLine> line1;
  line1->GetPointIds()->SetId(0, 1);
  line1->GetPointIds()->SetId(1, 2);

  vtkNew<vtkLine> line2;
  line2->GetPointIds()->SetId(0, 2);
  line2->GetPointIds()->SetId(1, 3);

  vtkNew<vtkCellArray> lines;
  lines->InsertNextCell(line0);
  lines->InsertNextCell(line1);
  lines->InsertNextCell(line2);

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->SetLines(lines);

  // Tell the polydata to build 'upward' links from points to cells.
  polydata->BuildLinks();
  // Mark a cell as deleted.
  polydata->DeleteCell(1);
  // Remove the marked cell.
  polydata->RemoveDeletedCells();

  // Visualize
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputDataObject(polydata);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());
  actor->GetProperty()->SetLineWidth(4);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("DeleteCells");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
