/*
This code is based on the VTK file: Examples/Modelling/Tcl/DelMesh.py.

This example demonstrates how to use 2D Delaunay triangulation.
We create a fancy image of a 2D Delaunay triangulation. Points are
 randomly generated.
*/

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDelaunay2D.h>
#include <vtkExtractEdges.h>
#include <vtkGlyph3D.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTubeFilter.h>

// Note that:
// vtkExtractEdges moved from vtkFiltersExtraction to vtkFiltersCore in
// VTK commit d9981b9aeb93b42d1371c6e295d76bfdc18430bd

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Generate some "random" points.
  vtkNew<vtkPoints> points;
  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(1);
  for (auto i = 0; i < 50; ++i)
  {
    double p1;
    double p2;
    p1 = randomSequence->GetValue();
    randomSequence->Next();
    p2 = randomSequence->GetValue();
    randomSequence->Next();
    points->InsertPoint(i, p1, p2, 0.0);
  }
  // Create a polydata with the points we just created.
  vtkNew<vtkPolyData> profile;
  profile->SetPoints(points);

  // Perform a 2D Delaunay triangulation on them.
  vtkNew<vtkDelaunay2D> delny;
  delny->SetInputData(profile);
  delny->SetTolerance(0.001);
  vtkNew<vtkPolyDataMapper> mapMesh;
  mapMesh->SetInputConnection(delny->GetOutputPort());
  vtkNew<vtkActor> meshActor;
  meshActor->SetMapper(mapMesh);
  meshActor->GetProperty()->SetColor(
      colors->GetColor3d("MidnightBlue").GetData());

  // We will now create a nice looking mesh by wrapping the edges in tubes,
  // and putting fat spheres at the points.
  vtkNew<vtkExtractEdges> extract;
  extract->SetInputConnection(delny->GetOutputPort());
  vtkNew<vtkTubeFilter> tubes;
  tubes->SetInputConnection(extract->GetOutputPort());
  tubes->SetRadius(0.01);
  tubes->SetNumberOfSides(6);
  vtkNew<vtkPolyDataMapper> mapEdges;
  mapEdges->SetInputConnection(tubes->GetOutputPort());
  vtkNew<vtkActor> edgeActor;
  edgeActor->SetMapper(mapEdges);
  edgeActor->GetProperty()->SetColor(colors->GetColor3d("peacock").GetData());
  edgeActor->GetProperty()->SetSpecularColor(1, 1, 1);
  edgeActor->GetProperty()->SetSpecular(0.3);
  edgeActor->GetProperty()->SetSpecularPower(20);
  edgeActor->GetProperty()->SetAmbient(0.2);
  edgeActor->GetProperty()->SetDiffuse(0.8);

  vtkNew<vtkSphereSource> ball;
  ball->SetRadius(0.025);
  ball->SetThetaResolution(12);
  ball->SetPhiResolution(12);
  vtkNew<vtkGlyph3D> balls;
  balls->SetInputConnection(delny->GetOutputPort());
  balls->SetSourceConnection(ball->GetOutputPort());
  vtkNew<vtkPolyDataMapper> mapBalls;
  mapBalls->SetInputConnection(balls->GetOutputPort());
  vtkNew<vtkActor> ballActor;
  ballActor->SetMapper(mapBalls);
  ballActor->GetProperty()->SetColor(colors->GetColor3d("hot_pink").GetData());
  ballActor->GetProperty()->SetSpecularColor(1, 1, 1);
  ballActor->GetProperty()->SetSpecular(0.3);
  ballActor->GetProperty()->SetSpecularPower(20);
  ballActor->GetProperty()->SetAmbient(0.2);
  ballActor->GetProperty()->SetDiffuse(0.8);

  // Create the rendering window, renderer, and interactive renderer.
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size.
  ren->AddActor(ballActor);
  ren->AddActor(edgeActor);
  ren->SetBackground(colors->GetColor3d("AliceBlue").GetData());
  renWin->SetSize(512, 512);
  renWin->SetWindowName("DelaunayMesh");

  ren->ResetCamera();
  ren->GetActiveCamera()->Zoom(1.3);

  // Interact with the data.
  iren->Initialize();
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
