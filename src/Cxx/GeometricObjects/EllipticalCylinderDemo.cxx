#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTubeFilter.h>

int main(int argc, char* argv[])
{
  double nx = 0.0;
  double ny = 0.0;
  double nz = 100.0;

  if (argc > 3)
  {
    nx = atof(argv[1]);
    ny = atof(argv[2]);
    nz = atof(argv[3]);
  }
  vtkNew<vtkNamedColors> colors;

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
  extrude->SetVector(nx, ny, nz);
  extrude->Update();

  // Create an oriented arrow
  double startPoint[3], endPoint[3];
  startPoint[0] = centerX;
  startPoint[1] = centerY;
  startPoint[2] = 0.0;
  for (auto i = 0; i < 3; ++i)
  {
    endPoint[i] = startPoint[i] + extrude->GetVector()[i];
  }

  // Compute a basis
  double normalizedX[3];
  double normalizedY[3];
  double normalizedZ[3];

  // The X axis is a vector from start to end
  vtkMath::Subtract(endPoint, startPoint, normalizedX);
  double length = vtkMath::Norm(normalizedX);
  vtkMath::Normalize(normalizedX);

  vtkNew<vtkMinimalStandardRandomSequence> rng;
  rng->SetSeed(8775070);
  auto max_r = 10.0;

  // The Z axis is an arbitrary vector cross X
  double arbitrary[3];
  for (auto i = 0; i < 3; ++i)
  {
    arbitrary[i] = rng->GetRangeValue(-max_r, max_r);
    rng->Next();
  }
  vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
  vtkMath::Normalize(normalizedZ);

  // The Y axis is Z cross X
  vtkMath::Cross(normalizedZ, normalizedX, normalizedY);
  vtkNew<vtkMatrix4x4> matrix;

  // Create the direction cosine matrix
  matrix->Identity();
  for (unsigned int i = 0; i < 3; i++)
  {
    matrix->SetElement(i, 0, normalizedX[i]);
    matrix->SetElement(i, 1, normalizedY[i]);
    matrix->SetElement(i, 2, normalizedZ[i]);
  }

  // Apply the transforms
  vtkNew<vtkTransform> transform;
  transform->Translate(startPoint);
  transform->Concatenate(matrix);
  transform->Scale(length, length, length);

  vtkNew<vtkArrowSource> arrowSource;
  arrowSource->SetTipResolution(31);
  arrowSource->SetShaftResolution(21);

  // Transform the polydata
  vtkNew<vtkTransformPolyDataFilter> transformPD;
  transformPD->SetTransform(transform);
  transformPD->SetInputConnection(arrowSource->GetOutputPort());

  // Create a mapper and actor for the arrow
  vtkNew<vtkPolyDataMapper> arrowMapper;
  arrowMapper->SetInputConnection(transformPD->GetOutputPort());

  vtkNew<vtkActor> arrowActor;
  arrowActor->SetMapper(arrowMapper);
  arrowActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkTubeFilter> tubes;
  tubes->SetInputData(polyData);
  tubes->SetRadius(2.0);
  tubes->SetNumberOfSides(21);

  vtkNew<vtkPolyDataMapper> lineMapper;
  lineMapper->SetInputConnection(tubes->GetOutputPort());

  vtkNew<vtkActor> lineActor;
  lineActor->SetMapper(lineMapper);
  lineActor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(extrude->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
  actor->GetProperty()->SetOpacity(.7);

  vtkNew<vtkRenderer> ren;
  ren->SetBackground(colors->GetColor3d("SlateGray").GetData());
  ren->AddActor(actor);
  ren->AddActor(lineActor);
  ren->AddActor(arrowActor);

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetWindowName("EllipticalCylinderDemo");
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
