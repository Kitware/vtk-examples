#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create five points.
  double origin[3] = {0.0, 0.0, 0.0};
  double p0[3] = {1.0, 0.0, 0.0};
  double p1[3] = {0.0, 1.0, 0.0};
  double p2[3] = {0.0, 1.0, 2.0};
  double p3[3] = {1.0, 2.0, 3.0};

  // Create a vtkPoints object and store the points in it
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(origin);
  points->InsertNextPoint(p0);
  points->InsertNextPoint(p1);
  points->InsertNextPoint(p2);
  points->InsertNextPoint(p3);

  vtkNew<vtkPolyLine> polyLine;
  polyLine->GetPointIds()->SetNumberOfIds(5);
  for (unsigned int i = 0; i < 5; i++)
  {
    polyLine->GetPointIds()->SetId(i, i);
  }

  // Create a cell array to store the lines in and add the lines to it
  vtkNew<vtkCellArray> cells;
  cells->InsertNextCell(polyLine);

  // Create a polydata to store everything in
  vtkNew<vtkPolyData> polyData;

  // Add the points to the dataset
  polyData->SetPoints(points);

  // Add the lines to the dataset
  polyData->SetLines(cells);

  // Setup actor and mapper
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polyData);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("PolyLine");
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkOliveGreen").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
