#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRotationalExtrusionFilter.h>
#include <vtkStripper.h>
#include <vtkTubeFilter.h>

int main(int, char*[])
{
  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // create bottle profile
  //
  vtkNew<vtkPoints> points;
  points->InsertPoint(0, 0.01, 0.0, 0.0);
  points->InsertPoint(1, 1.5, 0.0, 0.0);
  points->InsertPoint(2, 1.5, 0.0, 3.5);
  points->InsertPoint(3, 1.25, 0.0, 3.75);
  points->InsertPoint(4, 0.75, 0.0, 4.00);
  points->InsertPoint(5, 0.6, 0.0, 4.35);
  points->InsertPoint(6, 0.7, 0.0, 4.65);
  points->InsertPoint(7, 1.0, 0.0, 4.75);
  points->InsertPoint(8, 1.0, 0.0, 5.0);
  points->InsertPoint(9, 0.2, 0.0, 5.0);

  vtkNew<vtkCellArray> lines;
  lines->InsertNextCell(10); // number of points
  lines->InsertCellPoint(0);
  lines->InsertCellPoint(1);
  lines->InsertCellPoint(2);
  lines->InsertCellPoint(3);
  lines->InsertCellPoint(4);
  lines->InsertCellPoint(5);
  lines->InsertCellPoint(6);
  lines->InsertCellPoint(7);
  lines->InsertCellPoint(8);
  lines->InsertCellPoint(9);

  vtkNew<vtkPolyData> profile;
  profile->SetPoints(points);
  profile->SetLines(lines);

  // extrude profile to make bottle
  //
  vtkNew<vtkRotationalExtrusionFilter> extrude;
  extrude->SetInputData(profile);
  extrude->SetResolution(60);

  vtkNew<vtkPolyDataMapper> map;
  map->SetInputConnection(extrude->GetOutputPort());

  vtkNew<vtkActor> bottle;
  bottle->SetMapper(map);
  bottle->GetProperty()->SetColor(colors->GetColor3d("Mint").GetData());

  // display the profile
  vtkNew<vtkStripper> stripper;
  stripper->SetInputData(profile);

  vtkNew<vtkTubeFilter> tubes;
  tubes->SetInputConnection(stripper->GetOutputPort());
  tubes->SetNumberOfSides(11);
  tubes->SetRadius(0.05);

  vtkNew<vtkPolyDataMapper> profileMapper;
  profileMapper->SetInputConnection(tubes->GetOutputPort());

  vtkNew<vtkActor> profileActor;
  profileActor->SetMapper(profileMapper);
  profileActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  // Add the actors to the renderer, set the background and size
  //
  renderer->AddActor(bottle);
  renderer->AddActor(profileActor);
  renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());

  renWin->SetSize(640, 480);
  renWin->SetWindowName("Bottle");
  renWin->Render();

  renderer->GetActiveCamera()->SetPosition(1, 0, 0);
  renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);

  // render the image
  //
  iren->Start();

  return EXIT_SUCCESS;
}
