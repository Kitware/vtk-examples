#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkLabeledDataMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>

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
  // Create a point set
  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(10);
  pointSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> pointMapper;
  pointMapper->SetInputConnection(pointSource->GetOutputPort());

  vtkNew<vtkActor> pointActor;
  pointActor->SetMapper(pointMapper);
  pointActor->GetProperty()->SetPointSize(1);
  pointActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  // Map the points to spheres
  auto sphereActor = PointToGlyph(pointSource->GetOutput()->GetPoints(), 0.02);
  sphereActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  vtkNew<vtkLabeledDataMapper> labelMapper;
  labelMapper->SetInputConnection(pointSource->GetOutputPort());
  labelMapper->GetLabelTextProperty()->SetColor(
      colors->GetColor3d("Magenta").GetData());

  vtkNew<vtkActor2D> labelActor;
  labelActor->SetMapper(labelMapper);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("LabeledDataMapper");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  // renderer->AddActor(pointActor);
  renderer->AddActor(sphereActor);
  renderer->AddActor(labelActor);

  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Render and interact
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
