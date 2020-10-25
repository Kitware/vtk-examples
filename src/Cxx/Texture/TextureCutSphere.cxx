//
// Cut an outer sphere to reveal an inner sphere.
//
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkImplicitTextureCoords.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlanes.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkStructuredPointsReader.h>
#include <vtkTexture.h>

int main(int /* argc */, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Hidden sphere.
  vtkNew<vtkSphereSource> sphere1;
  sphere1->SetRadius(0.5);

  vtkNew<vtkPolyDataMapper> innerMapper;
  innerMapper->SetInputConnection(sphere1->GetOutputPort());

  vtkNew<vtkActor> innerSphere;
  innerSphere->SetMapper(innerMapper);
  innerSphere->GetProperty()->SetColor(
      colors->GetColor3d("BlanchedAlmond").GetData());

  // Sphere to texture.
  vtkNew<vtkSphereSource> sphere2;
  sphere2->SetRadius(1.0);
  sphere2->SetPhiResolution(21);
  sphere2->SetThetaResolution(21);

  vtkNew<vtkPlanes> planes;

  double pts[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(2);
  points->SetPoint(0, pts);
  points->SetPoint(1, pts + 3);

  double nrms[6] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
  vtkNew<vtkDoubleArray> normals;
  normals->SetNumberOfComponents(3);
  normals->SetNumberOfTuples(2);
  normals->SetTuple(0, nrms);
  normals->SetTuple(1, nrms + 3);

  planes->SetPoints(points);
  planes->SetNormals(normals);

  vtkNew<vtkImplicitTextureCoords> tcoords;
  tcoords->SetInputConnection(sphere2->GetOutputPort());
  tcoords->SetRFunction(planes);

  vtkNew<vtkDataSetMapper> outerMapper;
  outerMapper->SetInputConnection(tcoords->GetOutputPort());

  vtkNew<vtkStructuredPointsReader> tmap;
  tmap->SetFileName(argv[1]);

  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(tmap->GetOutputPort());
  texture->InterpolateOff();
  texture->RepeatOff();

  vtkNew<vtkActor> outerSphere;
  outerSphere->SetMapper(outerMapper);
  outerSphere->SetTexture(texture);
  outerSphere->GetProperty()->SetColor(
      colors->GetColor3d("LightSalmon").GetData());

  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iren;
  vtkNew<vtkRenderer> aren;
  iren->SetRenderWindow(renWin);
  renWin->AddRenderer(aren);

  aren->AddActor(innerSphere);
  aren->AddActor(outerSphere);
  aren->SetBackground(colors->GetColor3d("SlateGray").GetData());
  aren->GetActiveCamera()->Azimuth(-30);
  aren->GetActiveCamera()->Elevation(-30);
  aren->ResetCamera();

  renWin->SetSize(500, 500);
  renWin->SetWindowName("TextureCutSphere");

  // Interact with the data.
  renWin->Render();

  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
