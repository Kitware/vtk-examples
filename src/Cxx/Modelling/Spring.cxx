#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRotationalExtrusionFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // create spring profile (a circle)
  //
  vtkNew<vtkPoints> points;
  points->InsertPoint(0, 1.0, 0.0, 0.0);
  points->InsertPoint(1, 1.0732, 0.0, -0.1768);
  points->InsertPoint(2, 1.25, 0.0, -0.25);
  points->InsertPoint(3, 1.4268, 0.0, -0.1768);
  points->InsertPoint(4, 1.5, 0.0, 0.00);
  points->InsertPoint(5, 1.4268, 0.0, 0.1768);
  points->InsertPoint(6, 1.25, 0.0, 0.25);
  points->InsertPoint(7, 1.0732, 0.0, 0.1768);

  vtkNew<vtkCellArray> poly;
  poly->InsertNextCell(8); // number of points
  poly->InsertCellPoint(0);
  poly->InsertCellPoint(1);
  poly->InsertCellPoint(2);
  poly->InsertCellPoint(3);
  poly->InsertCellPoint(4);
  poly->InsertCellPoint(5);
  poly->InsertCellPoint(6);
  poly->InsertCellPoint(7);

  vtkNew<vtkPolyData> profile;
  profile->SetPoints(points);
  profile->SetPolys(poly);

  // extrude profile to make spring
  //
  vtkNew<vtkRotationalExtrusionFilter> extrude;
  extrude->SetInputData(profile);
  extrude->SetResolution(360);
  extrude->SetTranslation(6);
  extrude->SetDeltaRadius(1.0);
  extrude->SetAngle(2160.0); // six revolutions

  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputConnection(extrude->GetOutputPort());
  normals->SetFeatureAngle(60);

  vtkNew<vtkPolyDataMapper> map;
  map->SetInputConnection(normals->GetOutputPort());

  vtkNew<vtkActor> spring;
  spring->SetMapper(map);
  spring->GetProperty()->SetColor(colors->GetColor3d("PowderBlue").GetData());
  spring->GetProperty()->SetDiffuse(0.7);
  spring->GetProperty()->SetSpecular(0.4);
  spring->GetProperty()->SetSpecularPower(20);
  spring->GetProperty()->BackfaceCullingOn();

  // Add the actors to the renderer, set the background and size
  //
  renderer->AddActor(spring);
  renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());
  renderWindow->SetSize(640, 512);
  renderWindow->SetWindowName("Spring");

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(90);

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
