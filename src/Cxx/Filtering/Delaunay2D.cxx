#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkDelaunay2D.h>
#include <vtkLine.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTriangle.h>
#include <vtkVertexGlyphFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a set of heights on a grid.
  // This is often called a "terrain map".
  vtkNew<vtkPoints> points;

  unsigned int gridSize = 10;
  for (unsigned int x = 0; x < gridSize; x++)
  {
    for (unsigned int y = 0; y < gridSize; y++)
    {
      points->InsertNextPoint(x, y, (x + y) / (y + 1));
    }
  }

  // Add the grid points to a polydata object
  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);

  // Triangulate the grid points
  vtkNew<vtkDelaunay2D> delaunay;
  delaunay->SetInputData(polydata);

  // Visualize
  vtkNew<vtkPolyDataMapper> meshMapper;
  meshMapper->SetInputConnection(delaunay->GetOutputPort());

  vtkNew<vtkActor> meshActor;
  meshActor->SetMapper(meshMapper);
  meshActor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
  meshActor->GetProperty()->EdgeVisibilityOn();

  vtkNew<vtkVertexGlyphFilter> glyphFilter;
  glyphFilter->SetInputData(polydata);

  vtkNew<vtkPolyDataMapper> pointMapper;
  pointMapper->SetInputConnection(glyphFilter->GetOutputPort());

  vtkNew<vtkActor> pointActor;
  pointActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
  pointActor->GetProperty()->SetPointSize(5);
  pointActor->SetMapper(pointMapper);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(meshActor);
  renderer->AddActor(pointActor);
  renderer->SetBackground(colors->GetColor3d("Mint").GetData());

  renderWindow->SetWindowName("Delaunay2D");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
