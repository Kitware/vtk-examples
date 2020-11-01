#include <vtkNew.h>
#include <vtkSmartPointer.h>

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkCellCenters.h>
#include <vtkIdFilter.h>
#include <vtkLabeledDataMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelectVisiblePoints.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>

namespace {
int xLength;
int yLength;
typedef struct
{
  vtkSmartPointer<vtkSelectVisiblePoints> visPts;
  vtkSmartPointer<vtkSelectVisiblePoints> visCells;
  vtkSmartPointer<vtkPoints> pts;
  vtkSmartPointer<vtkRenderWindow> renWin;
} VTKData;

// Create a procedure to draw the selection window at each location it
// is moved to.
void PlaceWindow(VTKData const& data, int xmin, int ymin);

// Create a procedure to move the selection window across the data set.
void MoveWindow(VTKData const& data);

} // namespace

int main(int /* argc */, char* /* argv */[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a selection window.  We will display the point and cell ids that
  // lie within this window.
  int xmin = 200;
  xLength = 100;
  int xmax = xmin + xLength;
  int ymin = 200;
  yLength = 100;
  int ymax = ymin + yLength;

  vtkNew<vtkPoints> pts;
  pts->InsertPoint(0, xmin, ymin, 0);
  pts->InsertPoint(1, xmax, ymin, 0);
  pts->InsertPoint(2, xmax, ymax, 0);
  pts->InsertPoint(3, xmin, ymax, 0);

  vtkNew<vtkCellArray> rect;
  rect->InsertNextCell(5);
  rect->InsertCellPoint(0);
  rect->InsertCellPoint(1);
  rect->InsertCellPoint(2);
  rect->InsertCellPoint(3);
  rect->InsertCellPoint(0);

  vtkNew<vtkPolyData> selectRect;
  selectRect->SetPoints(pts);
  selectRect->SetLines(rect);

  vtkNew<vtkPolyDataMapper2D> rectMapper;
  rectMapper->SetInputData(selectRect);

  vtkNew<vtkActor2D> rectActor;
  rectActor->SetMapper(rectMapper);
  rectActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  // Create a sphere and its associated mapper and actor.
  vtkNew<vtkSphereSource> sphere;
  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphere->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(
      colors->GetColor3d("BurlyWood").GetData());

  // Generate data arrays containing point and cell ids
  vtkNew<vtkIdFilter> ids;
  ids->SetInputConnection(sphere->GetOutputPort());
  ids->PointIdsOn();
  ids->CellIdsOn();
  ids->FieldDataOn();

  // Create the renderer here because vtkSelectVisiblePoints needs it.
  vtkNew<vtkRenderer> ren1;
  ren1->UseHiddenLineRemovalOn();

  // Create labels for points
  vtkNew<vtkSelectVisiblePoints> visPts;
  visPts->SetInputConnection(ids->GetOutputPort());
  visPts->SetRenderer(ren1);
  visPts->SelectionWindowOn();
  visPts->SetSelection(xmin, xmin + xLength, ymin, ymin + yLength);

  // Create the mapper to display the point ids.  Specify the
  // format to use for the labels.  Also create the associated actor.
  vtkNew<vtkLabeledDataMapper> ldm;
  ldm->SetInputConnection(visPts->GetOutputPort());
  ldm->SetLabelModeToLabelFieldData();

  vtkNew<vtkActor2D> pointLabels;
  pointLabels->SetMapper(ldm);
  pointLabels->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

  // Create labels for cells
  vtkNew<vtkCellCenters> cc;
  cc->SetInputConnection(ids->GetOutputPort());

  vtkNew<vtkSelectVisiblePoints> visCells;
  visCells->SetInputConnection(cc->GetOutputPort());
  visCells->SetRenderer(ren1);
  visCells->SelectionWindowOn();
  visCells->SetSelection(xmin, xmin + xLength, ymin, ymin + yLength);

  // Create the mapper to display the cell ids.  Specify the
  // format to use for the labels.  Also create the associated actor.
  vtkNew<vtkLabeledDataMapper> cellMapper;
  cellMapper->SetInputConnection(visCells->GetOutputPort());
  cellMapper->SetLabelModeToLabelFieldData();
  cellMapper->GetLabelTextProperty()->SetColor(
      colors->GetColor3d("DarkGreen").GetData());

  vtkNew<vtkActor2D> cellLabels;
  cellLabels->SetMapper(cellMapper);

  // Create the RenderWindow and RenderWindowInteractor
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer; set the background and size; render
  ren1->AddActor(sphereActor);
  ren1->AddActor2D(rectActor);
  ren1->AddActor2D(pointLabels);
  ren1->AddActor2D(cellLabels);
  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renWin->SetSize(500, 500);
  renWin->SetWindowName("LabeledMesh");

  renWin->Render();

  VTKData data = {visPts, visCells, pts, renWin};

  // Move the selection window across the data set.
  MoveWindow(data);

  // Put the selection window in the center of the render window.
  // This works because the xmin = ymin = 200, xLength = yLength = 100, and
  // the render window size is 500 x 500.
  PlaceWindow(data, xmin, ymin);

  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {
void PlaceWindow(VTKData const& data, int xmin, int ymin)
{
  int xmax = xmin + xLength;
  int ymax = ymin + yLength;

  data.visPts->SetSelection(xmin, xmax, ymin, ymax);
  data.visCells->SetSelection(xmin, xmax, ymin, ymax);

  data.pts->InsertPoint(0, xmin, ymin, 0);
  data.pts->InsertPoint(1, xmin, ymin, 0);
  data.pts->InsertPoint(2, xmin, ymin, 0);
  data.pts->InsertPoint(3, xmin, ymin, 0);

  // Call Modified because InsertPoints does not modify vtkPoints
  // (for performance reasons).
  data.pts->Modified();

  data.renWin->Render();
}

void MoveWindow(VTKData const& data)
{
  for (int y = 100; y < 300; y += 25)
  {
    for (int x = 100; x < 300; x += 25)
    {
      PlaceWindow(data, x, y);
    }
  }
}
} // namespace
