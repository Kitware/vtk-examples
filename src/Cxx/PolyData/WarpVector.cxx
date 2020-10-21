#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkLine.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkWarpVector.h>

int main(int, char*[])
{

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(2.0, 0.0, 0.0);
  points->InsertNextPoint(3.0, 0.0, 0.0);
  points->InsertNextPoint(4.0, 0.0, 0.0);

  vtkNew<vtkCellArray> lines;
  vtkNew<vtkLine> line;
  line->GetPointIds()->SetId(0, 0);
  line->GetPointIds()->SetId(1, 1);
  lines->InsertNextCell(line);
  line->GetPointIds()->SetId(0, 1);
  line->GetPointIds()->SetId(1, 2);
  lines->InsertNextCell(line);
  line->GetPointIds()->SetId(0, 2);
  line->GetPointIds()->SetId(1, 3);
  lines->InsertNextCell(line);
  line->GetPointIds()->SetId(0, 3);
  line->GetPointIds()->SetId(1, 4);
  lines->InsertNextCell(line);

  vtkNew<vtkDoubleArray> warpData;
  warpData->SetNumberOfComponents(3);
  warpData->SetName("warpData");
  double warp[3] = {0.0, 0.0, 0.0};
  warp[1] = 0.0;
  warpData->InsertNextTuple(warp);
  warp[1] = 0.1;
  warpData->InsertNextTuple(warp);
  warp[1] = 0.3;
  warpData->InsertNextTuple(warp);
  warp[1] = 0.0;
  warpData->InsertNextTuple(warp);
  warp[1] = 0.1;
  warpData->InsertNextTuple(warp);

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->SetLines(lines);
  polydata->GetPointData()->AddArray(warpData);
  polydata->GetPointData()->SetActiveVectors(warpData->GetName());

  // WarpVector will use the array marked as active vector in polydata
  // it has to be a 3 component array
  // with the same number of tuples as points in polydata
  vtkNew<vtkWarpVector> warpVector;
  warpVector->SetInputData(polydata);
  warpVector->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(warpVector->GetPolyDataOutput());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("cobalt_green").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("WarpVector");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
