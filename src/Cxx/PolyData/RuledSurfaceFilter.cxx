#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRuledSurfaceFilter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create first line.
  vtkNew<vtkPoints> points;
  points->InsertPoint(0, 0, 0, 1);
  points->InsertPoint(1, 1, 0, 0);
  points->InsertPoint(2, 0, 1, 0);
  points->InsertPoint(3, 1, 1, 1);

  vtkNew<vtkLine> line1;
  line1->GetPointIds()->SetId(0, 0);
  line1->GetPointIds()->SetId(1, 1);

  vtkNew<vtkLine> line2;
  line2->GetPointIds()->SetId(0, 2);
  line2->GetPointIds()->SetId(1, 3);

  vtkNew<vtkCellArray> lines;
  lines->InsertNextCell(line1);
  lines->InsertNextCell(line2);

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->SetLines(lines);

  vtkNew<vtkRuledSurfaceFilter> ruledSurfaceFilter;
  ruledSurfaceFilter->SetInputData(polydata);
  ruledSurfaceFilter->SetResolution(21, 21);
  ruledSurfaceFilter->SetRuledModeToResample();

  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("RuledSurfaceFilter");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(ruledSurfaceFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());

  // Add the actors to the renderer, set the background and size
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(60);
  renderer->GetActiveCamera()->Elevation(60);
  renderer->GetActiveCamera()->Dolly(1.0);
  renderer->ResetCameraClippingRange();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
