#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkNamedColors.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkNew.h>

int main(int, char *[])
{
  vtkNew<vtkNamedColors> colors;

  // Create the geometry of a point (the coordinate)
  vtkNew<vtkPoints> points;
  const float p[3] = {1.0, 2.0, 3.0};

  // Create the topology of the point (a vertex)
  vtkNew<vtkCellArray> vertices;
  // We need an an array of point id's for InsertNextCell.
  vtkIdType pid[1];
  pid[0] = points->InsertNextPoint(p);
  vertices->InsertNextCell(1, pid);

  // Create a polydata object
  vtkNew<vtkPolyData> point;

  // Set the points and vertices we created as the geometry and topology of the
  // polydata
  point->SetPoints(points);
  point->SetVerts(vertices);

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(point);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
  actor->GetProperty()->SetPointSize(20);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("Point");
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkGreen").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();
  
  return EXIT_SUCCESS;
}
