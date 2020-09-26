#include <vtkActor2D.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVertexGlyphFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPoints> points;
  points->InsertNextPoint(10, 10, 0);
  points->InsertNextPoint(100, 100, 0);
  points->InsertNextPoint(200, 200, 0);

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);

  vtkNew<vtkVertexGlyphFilter> glyphFilter;
  glyphFilter->SetInputData(polydata);
  glyphFilter->Update();

  vtkNew<vtkPolyDataMapper2D> mapper;
  mapper->SetInputConnection(glyphFilter->GetOutputPort());
  mapper->Update();

  vtkNew<vtkActor2D> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());
  actor->GetProperty()->SetPointSize(8);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  renderWindow->SetSize(300, 300);
  renderWindow->SetWindowName("Actor2D");

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
