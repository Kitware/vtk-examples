#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkGlyph2D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 1, 0);
  points->InsertNextPoint(2, 2, 0);

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);

  // Create anything you want here, we will use a polygon for the demo.
  vtkNew<vtkRegularPolygonSource> polygonSource; // default is 6 sides

  vtkNew<vtkGlyph2D> glyph2D;
  glyph2D->SetSourceConnection(polygonSource->GetOutputPort());
  glyph2D->SetInputData(polydata);
  glyph2D->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(glyph2D->GetOutputPort());
  mapper->Update();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Salmon").GetData());

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkInteractorStyleImage> style;
  renderWindowInteractor->SetInteractorStyle(style);

  renderWindow->SetWindowName("Glyph2D");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
