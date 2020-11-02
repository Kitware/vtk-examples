#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCutter.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkHexahedron.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>

#include <array>
#include <cstdlib>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Setup the coordinates of eight points
  // (the two faces must be in counter clockwise order as viewed from the
  // outside)
  std::array<std::array<double, 3>, 8> pointCoords{{{{0.0, 0.0, 0.0}},
                                                    {{1.0, 0.0, 0.0}},
                                                    {{1.0, 1.0, 0.0}},
                                                    {{0.0, 1.0, 0.0}},
                                                    {{0.0, 0.0, 1.0}},
                                                    {{1.0, 0.0, 1.0}},
                                                    {{1.0, 1.0, 1.0}},
                                                    {{0.0, 1.0, 1.0}}}};

  // Create the points and a hexahedron from the points.
  vtkNew<vtkPoints> points;
  vtkNew<vtkHexahedron> hexa;
  for (auto i = 0; i < pointCoords.size(); ++i)
  {
    points->InsertNextPoint(pointCoords[i].data());
    hexa->GetPointIds()->SetId(i, i);
  }

  // Add the hexahedron to a cell array.
  vtkNew<vtkCellArray> hexs;
  hexs->InsertNextCell(hexa);

  // Add the points and hexahedron to an unstructured grid.
  vtkNew<vtkUnstructuredGrid> uGrid;
  uGrid->SetPoints(points);
  uGrid->InsertNextCell(hexa->GetCellType(), hexa->GetPointIds());

  // Extract the outer (polygonal) surface.
  vtkNew<vtkDataSetSurfaceFilter> surface;
  surface->SetInputData(uGrid);
  surface->Update();

  vtkNew<vtkDataSetMapper> aBeamMapper;
  aBeamMapper->SetInputConnection(surface->GetOutputPort());
  vtkNew<vtkActor> aBeamActor;
  aBeamActor->SetMapper(aBeamMapper);
  aBeamActor->AddPosition(0, 0, 0);
  aBeamActor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());
  aBeamActor->GetProperty()->SetOpacity(0.60);
  aBeamActor->GetProperty()->EdgeVisibilityOn();
  aBeamActor->GetProperty()->SetEdgeColor(
      colors->GetColor3d("Black").GetData());
  aBeamActor->GetProperty()->SetLineWidth(1.5);

  // Create a plane to cut, here it cuts in the XZ direction
  // (xz normal=(1,0,0); XY =(0,0,1), YZ =(0,1,0)
  vtkNew<vtkPlane> plane;
  plane->SetOrigin(0.5, 0, 0);
  plane->SetNormal(1, 0, 0);

  // Create cutter
  vtkNew<vtkCutter> cutter;
  cutter->SetCutFunction(plane);
  cutter->SetInputData(aBeamActor->GetMapper()->GetInput());
  cutter->Update();
  vtkNew<vtkPolyDataMapper> cutterMapper;
  cutterMapper->SetInputConnection(cutter->GetOutputPort());

  // Create plane actor
  vtkNew<vtkActor> planeActor;
  planeActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
  planeActor->GetProperty()->SetLineWidth(2);
  planeActor->SetMapper(cutterMapper);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("DatasetSurface");
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(aBeamActor);
  renderer->AddActor(planeActor);
  renderer->SetBackground(colors->GetColor3d("Seashell").GetData());

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(-25);
  renderer->GetActiveCamera()->Elevation(30);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
