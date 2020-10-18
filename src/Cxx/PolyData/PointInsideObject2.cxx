#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDelaunay3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> bigSphereSource;
  bigSphereSource->SetCenter(0.0, 0.0, 0.0);
  bigSphereSource->SetRadius(4);
  bigSphereSource->SetThetaResolution(20);
  bigSphereSource->SetPhiResolution(20);
  bigSphereSource->Update();

  vtkNew<vtkPolyDataMapper> bigSphereMapper;
  bigSphereMapper->SetInputConnection(bigSphereSource->GetOutputPort());

  vtkNew<vtkActor> bigSphereActor;
  bigSphereActor->SetMapper(bigSphereMapper);
  bigSphereActor->GetProperty()->SetOpacity(0.5);
  bigSphereActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  vtkNew<vtkSphereSource> smallSphereSource;
  smallSphereSource->SetCenter(1, 2, 1);
  smallSphereSource->SetRadius(2);
  smallSphereSource->SetThetaResolution(20);
  smallSphereSource->SetPhiResolution(20);
  smallSphereSource->Update();

  vtkNew<vtkPolyDataMapper> smallSphereMapper;
  smallSphereMapper->SetInputConnection(smallSphereSource->GetOutputPort());

  vtkNew<vtkActor> smallSphereActor;
  smallSphereActor->SetMapper(smallSphereMapper);
  smallSphereActor->GetProperty()->SetColor(
      colors->GetColor3d("Cornsilk").GetData());

  vtkNew<vtkPoints> pointsInside;
  vtkNew<vtkPolyData> polyDataInside;
  double point[3];

  // We generate points inside a spherical area defined by bigSphereSource.
  // Then we apply delaunay3D to it instead on  bigSphereSource.
  // This will avoid the "Unable to factor linear system" warnings cuused by
  //  degenerate points on the sphere.
  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(400);
  // Slightly larger than the radius of the big sphere
  pointSource->SetRadius(4.2);

  vtkNew<vtkSmoothPolyDataFilter> smoothFilter;
  smoothFilter->SetInputConnection(0, bigSphereSource->GetOutputPort());
  smoothFilter->SetInputConnection(1, pointSource->GetOutputPort());

  vtkNew<vtkDelaunay3D> delaunay3D;
  // delaunay3D->SetInputConnection(bigSphereSource->GetOutputPort());
  delaunay3D->SetInputConnection(smoothFilter->GetOutputPort());
  delaunay3D->Update();

  for (vtkIdType i = 0; i < smallSphereSource->GetOutput()->GetNumberOfPoints();
       i++)
  {
    smallSphereSource->GetOutput()->GetPoint(i, point);

    double pcoords[3];
    // The number of weights >= max number of nodes in each cell
    double weights[4];
    int subId;
    vtkIdType cellId = delaunay3D->GetOutput()->FindCell(
        point, nullptr, 0, 0.1, subId, pcoords, weights);

    if (cellId >= 0)
    {
      std::cout << "In cell " << cellId << std::endl;
      std::cout << "inside" << std::endl;
      pointsInside->InsertNextPoint(point);
    }
    else
    {
      std::cout << "outside" << std::endl;
    }
  }

  polyDataInside->SetPoints(pointsInside);

  vtkNew<vtkVertexGlyphFilter> vertexGlyphFilter;
  vertexGlyphFilter->SetInputData(polyDataInside);
  vertexGlyphFilter->Update();

  // Mapper and actor for points inside
  vtkNew<vtkPolyDataMapper> pointsInsideMapper;
  pointsInsideMapper->SetInputConnection(vertexGlyphFilter->GetOutputPort());
  vtkNew<vtkActor> pointsInsideActor;
  pointsInsideActor->SetMapper(pointsInsideMapper);
  pointsInsideActor->GetProperty()->SetPointSize(6);
  pointsInsideActor->GetProperty()->SetColor(
      colors->GetColor3d("Magenta").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("PointInsideObject2");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(smallSphereActor);
  renderer->AddActor(bigSphereActor);
  renderer->AddActor(pointsInsideActor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
