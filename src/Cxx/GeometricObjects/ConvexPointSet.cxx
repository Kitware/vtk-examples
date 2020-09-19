#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkConvexPointSet.h>
#include <vtkDataSetMapper.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShrinkFilter.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>

int main(int, char*[])
{
  vtkNew<vtkConvexPointSet> cps;
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 0, 0);
  points->InsertNextPoint(1, 1, 0);
  points->InsertNextPoint(0, 1, 0);
  points->InsertNextPoint(0, 0, 1);
  points->InsertNextPoint(1, 0, 1);
  points->InsertNextPoint(1, 1, 1);
  points->InsertNextPoint(0, 1, 1);
  points->InsertNextPoint(0.5, 0, 0);
  points->InsertNextPoint(1, 0.5, 0);
  points->InsertNextPoint(0.5, 1, 0);
  points->InsertNextPoint(0, 0.5, 0);
  points->InsertNextPoint(0.5, 0.5, 0);

  for (int i = 0; i < 13; ++i)
  {
    cps->GetPointIds()->InsertId(i, i);
  }

  vtkNew<vtkUnstructuredGrid> ug;
  ug->Allocate(1, 1);
  ug->InsertNextCell(cps->GetCellType(), cps->GetPointIds());
  ug->SetPoints(points);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputData(ug);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
  actor->GetProperty()->SetLineWidth(3);
  actor->GetProperty()->EdgeVisibilityOn();

  // Glyph the points
  vtkNew<vtkSphereSource> sphere;
  sphere->SetPhiResolution(21);
  sphere->SetThetaResolution(21);
  sphere->SetRadius(.03);

  // Create a polydata to store everything in
  vtkNew<vtkPolyData> polyData;
  polyData->SetPoints(points);

  vtkNew<vtkGlyph3DMapper> pointMapper;
  pointMapper->SetInputData(polyData);
  pointMapper->SetSourceConnection(sphere->GetOutputPort());

  vtkNew<vtkActor> pointActor;
  pointActor->SetMapper(pointMapper);
  pointActor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("ConvexPointSet");
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->AddActor(pointActor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(210);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCameraClippingRange();

  // Render and interact
  renderWindow->SetSize(640, 480);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
