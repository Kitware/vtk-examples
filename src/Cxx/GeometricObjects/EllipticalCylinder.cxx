#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int /* argc */, char* /* argv */[])
{

  double angle = 0;
  double r1, r2;
  double centerX, centerY;
  r1 = 50;
  r2 = 30;
  centerX = 10.0;
  centerY = 5.0;

  vtkNew<vtkPoints> points;
  int id = 0;
  while (angle <= 2.0 * vtkMath::Pi() + (vtkMath::Pi() / 60.0))
  {
    points->InsertNextPoint(r1 * cos(angle) + centerX,
                            r2 * sin(angle) + centerY, 0.0);
    angle = angle + (vtkMath::Pi() / 60.0);
    ++id;
  }

  vtkNew<vtkPolyLine> line;
  line->GetPointIds()->SetNumberOfIds(id);
  for (unsigned int i = 0; i < static_cast<unsigned int>(id); ++i)
  {
    line->GetPointIds()->SetId(i, i);
  }

  vtkNew<vtkCellArray> lines;
  lines->InsertNextCell(line);

  vtkNew<vtkPolyData> polyData;
  polyData->SetPoints(points);
  polyData->SetLines(lines);

  vtkNew<vtkLinearExtrusionFilter> extrude;
  extrude->SetInputData(polyData);
  extrude->SetExtrusionTypeToNormalExtrusion();
  extrude->SetVector(0, 0, 100.0);
  extrude->Update();

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> lineMapper;
  lineMapper->SetInputData(polyData);

  vtkNew<vtkActor> lineActor;
  lineActor->SetMapper(lineMapper);
  lineActor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(extrude->GetOutputPort());

  vtkNew<vtkProperty> back;
  back->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
  actor->SetBackfaceProperty(back);

  vtkNew<vtkRenderer> ren;
  ren->SetBackground(colors->GetColor3d("SlateGray").GetData());
  ren->AddActor(actor);
  ren->AddActor(lineActor);

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetWindowName("EllipticalCylinder");
  renWin->AddRenderer(ren);
  renWin->SetSize(600, 600);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  iren->SetInteractorStyle(style);

  vtkNew<vtkCamera> camera;
  camera->SetPosition(0, 1, 0);
  camera->SetFocalPoint(0, 0, 0);
  camera->SetViewUp(0, 0, 1);
  camera->Azimuth(30);
  camera->Elevation(30);

  ren->SetActiveCamera(camera);
  ren->ResetCamera();
  ren->ResetCameraClippingRange();

  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
