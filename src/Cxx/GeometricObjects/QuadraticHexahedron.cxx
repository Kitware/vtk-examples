#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkGlyph3D.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkQuadraticHexahedron.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkSphereSource.h>
#include <vtkTessellatorFilter.h>
#include <vtkUnstructuredGrid.h>

namespace {
vtkSmartPointer<vtkUnstructuredGrid> MakeQuadraticHexahedron();
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> namedColors;

  auto uGrid = MakeQuadraticHexahedron();

  vtkNew<vtkTessellatorFilter> tessellate;
  tessellate->SetInputData(uGrid);
  tessellate->SetMaximumNumberOfSubdivisions(2);

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(tessellate->GetOutputPort());
  mapper->ScalarVisibilityOff();

  // Create an actor for the grid
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetDiffuseColor(
      namedColors->GetColor3d("Tomato").GetData());
  actor->GetProperty()->SetEdgeColor(
      namedColors->GetColor3d("IvoryBlack").GetData());
  actor->GetProperty()->EdgeVisibilityOn();

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(0.02);

  vtkNew<vtkGlyph3D> glyph3D;
  glyph3D->SetInputData(uGrid);
  glyph3D->SetSourceConnection(sphereSource->GetOutputPort());
  glyph3D->ScalingOff();
  glyph3D->Update();

  vtkNew<vtkDataSetMapper> glyph3DMapper;
  glyph3DMapper->SetInputConnection(glyph3D->GetOutputPort());
  glyph3DMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> glyph3DActor;
  glyph3DActor->SetMapper(glyph3DMapper);
  glyph3DActor->GetProperty()->SetColor(
      namedColors->GetColor3d("Banana").GetData());

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("QuadraticHexahedron");
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->AddActor(glyph3DActor);
  renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkUnstructuredGrid> MakeQuadraticHexahedron()
{
  vtkNew<vtkQuadraticHexahedron> aHexahedron;
  vtkNew<vtkPoints> points;

  double* pcoords = aHexahedron->GetParametricCoords();
  vtkNew<vtkMinimalStandardRandomSequence> rng;
  points->SetNumberOfPoints(aHexahedron->GetNumberOfPoints());
  rng->SetSeed(5070); // for testing
  for (auto i = 0; i < aHexahedron->GetNumberOfPoints(); ++i)
  {
    double perturbation[3];
    for (auto j = 0; j < 3; ++j)
    {
      rng->Next();
      perturbation[j] = rng->GetRangeValue(-0.1, 0.1);
    }
    aHexahedron->GetPointIds()->SetId(i, i);
    points->SetPoint(i, *(pcoords + 3 * i) + perturbation[0],
                     *(pcoords + 3 * i + 1) + perturbation[1],
                     *(pcoords + 3 * i + 2) + perturbation[2]);
  }

  // Add the points and hexahedron to an unstructured grid
  vtkSmartPointer<vtkUnstructuredGrid> uGrid =
      vtkSmartPointer<vtkUnstructuredGrid>::New();
  uGrid->SetPoints(points);
  uGrid->InsertNextCell(aHexahedron->GetCellType(), aHexahedron->GetPointIds());

  return uGrid;
}
} // namespace
