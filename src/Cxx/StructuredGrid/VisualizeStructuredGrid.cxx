#include <vtkActor.h>
#include <vtkGlyph3DMapper.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkXMLStructuredGridWriter.h>

namespace {
/**
 * Convert points to glyphs.
 *
 * @param points - The points to glyph
 * @param scale - The scale, used to determine the size of the glyph
 * representing the point, expressed as a fraction of the largest side of the
 * bounding box surrounding the points. e.g. 0.05
 *
 * @return The actor.
 */
vtkSmartPointer<vtkActor> PointToGlyph(vtkPoints* points, double const& scale);

} // namespace
int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a grid
  vtkNew<vtkStructuredGrid> structuredGrid;

  vtkNew<vtkPoints> points;
  unsigned int numi = 2;
  unsigned int numj = 3;
  unsigned int numk = 2;

  for (unsigned int k = 0; k < numk; k++)
  {
    for (unsigned int j = 0; j < numj; j++)
    {
      for (unsigned int i = 0; i < numi; i++)
      {
        points->InsertNextPoint(i, j, k);
      }
    }
  }

  // Specify the dimensions of the grid
  structuredGrid->SetDimensions(numi, numj, numk);
  structuredGrid->SetPoints(points);

  std::cout << "There are " << structuredGrid->GetNumberOfPoints() << " points."
            << std::endl; // there should be 2*3*2 = 12 points
  std::cout << "There are " << structuredGrid->GetNumberOfCells() << " cells."
            << std::endl; // The 12 points define the corners of 2 cubes/cells
                          // (4 points are shared by both cubes)

  vtkNew<vtkStructuredGridGeometryFilter> geometryFilter;
  geometryFilter->SetInputData(structuredGrid);
  geometryFilter->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(geometryFilter->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(1);

  // Visualize

  // Map the points to spheres
  auto sphereActor =
      PointToGlyph(geometryFilter->GetOutput()->GetPoints(), 0.02);
  sphereActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("VisualizeStructuredGrid");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->AddActor(sphereActor);

  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

vtkSmartPointer<vtkActor> PointToGlyph(vtkPoints* points, double const& scale)
{
  auto bounds = points->GetBounds();
  double maxLen = 0;
  for (int i = 1; i < 3; ++i)
  {
    maxLen = std::max(bounds[i + 1] - bounds[i], maxLen);
  }

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(scale * maxLen);

  vtkNew<vtkPolyData> pd;
  pd->SetPoints(points);

  vtkNew<vtkGlyph3DMapper> mapper;
  mapper->SetInputData(pd);
  mapper->SetSourceConnection(sphereSource->GetOutputPort());
  mapper->ScalarVisibilityOff();
  mapper->ScalingOff();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  return actor;
}

} // namespace
