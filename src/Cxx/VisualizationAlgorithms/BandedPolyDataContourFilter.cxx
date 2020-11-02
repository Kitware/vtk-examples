#include <vtkActor.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPoints> pts;
  pts->InsertPoint(0, 0, 0, 0);
  pts->InsertPoint(1, 0, 1, 0);
  pts->InsertPoint(2, 0, 2, 0);
  pts->InsertPoint(3, 1, 0, 0);
  pts->InsertPoint(4, 1, 1, 0);
  pts->InsertPoint(5, 1, 2, 0);
  pts->InsertPoint(6, 2, 0, 0);
  pts->InsertPoint(7, 2, 2, 0);
  pts->InsertPoint(8, 3, 0, 0);
  pts->InsertPoint(9, 3, 1, 0);
  pts->InsertPoint(10, 3, 2, 0);
  pts->InsertPoint(11, 4, 0, 0);
  pts->InsertPoint(12, 6, 0, 0);
  pts->InsertPoint(13, 5, 2, 0);
  pts->InsertPoint(14, 7, 0, 0);
  pts->InsertPoint(15, 9, 0, 0);
  pts->InsertPoint(16, 7, 2, 0);
  pts->InsertPoint(17, 9, 2, 0);
  pts->InsertPoint(18, 10, 0, 0);
  pts->InsertPoint(19, 12, 0, 0);
  pts->InsertPoint(20, 10, 1, 0);
  pts->InsertPoint(21, 12, 1, 0);
  pts->InsertPoint(22, 10, 2, 0);
  pts->InsertPoint(23, 12, 2, 0);
  pts->InsertPoint(24, 10, 3, 0);
  pts->InsertPoint(25, 12, 3, 0);

  vtkNew<vtkCellArray> polys;
  polys->InsertNextCell(4);
  polys->InsertCellPoint(14);
  polys->InsertCellPoint(15);
  polys->InsertCellPoint(17);
  polys->InsertCellPoint(16);
  polys->InsertNextCell(3);
  polys->InsertCellPoint(11);
  polys->InsertCellPoint(12);
  polys->InsertCellPoint(13);

  vtkNew<vtkFloatArray> scalars;
  scalars->SetNumberOfTuples(26);
  scalars->SetTuple1(0, 0);
  scalars->SetTuple1(1, 50);
  scalars->SetTuple1(2, 100);
  scalars->SetTuple1(3, 0);
  scalars->SetTuple1(4, 50);
  scalars->SetTuple1(5, 100);
  scalars->SetTuple1(6, 10);
  scalars->SetTuple1(7, 90);
  scalars->SetTuple1(8, 10);
  scalars->SetTuple1(9, 50);
  scalars->SetTuple1(10, 90);
  scalars->SetTuple1(11, 10);
  scalars->SetTuple1(12, 40);
  scalars->SetTuple1(13, 100);
  scalars->SetTuple1(14, 0);
  scalars->SetTuple1(15, 60);
  scalars->SetTuple1(16, 40);
  scalars->SetTuple1(17, 100);
  scalars->SetTuple1(18, 0);
  scalars->SetTuple1(19, 25);
  scalars->SetTuple1(20, 25);
  scalars->SetTuple1(21, 50);
  scalars->SetTuple1(22, 50);
  scalars->SetTuple1(23, 75);
  scalars->SetTuple1(24, 75);
  scalars->SetTuple1(25, 100);

  vtkNew<vtkPolyData> polyData;
  polyData->SetPoints(pts);
  polyData->SetPolys(polys);
  polyData->GetPointData()->SetScalars(scalars);

  vtkNew<vtkBandedPolyDataContourFilter> bf;
  bf->SetInputData(polyData);
  bf->GenerateValues(5, 25, 75);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(bf->GetOutputPort());
  mapper->SetScalarModeToUseCellData();
  mapper->SetScalarRange(0, 4);
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Create the RenderWindow, Renderer and both Actors

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer, set the background and size
  renderer->AddActor(actor);

  renderWindow->SetSize(1200, 400);
  renderWindow->Render();
  renderWindow->SetWindowName("BandedPolyDataContourFilter");

  renderer->SetBackground(colors->GetColor3d("Melon").GetData());
  renderer->GetActiveCamera()->Zoom(2.5);

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
