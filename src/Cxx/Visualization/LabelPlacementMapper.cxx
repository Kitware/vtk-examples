#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkIntArray.h>
#include <vtkLabelPlacementMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkStringArray.h>
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

  // Create a point set.
  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(6);
  pointSource->Update();

  // Add label array.
  vtkNew<vtkStringArray> labels;
  labels->SetNumberOfValues(6);
  labels->SetName("labels");
  labels->SetValue(0, "Priority 10");
  labels->SetValue(1, "Priority 7");
  labels->SetValue(2, "Priority 6");
  labels->SetValue(3, "Priority 4");
  labels->SetValue(4, "Priority 4");
  labels->SetValue(5, "Priority 4");
  pointSource->GetOutput()->GetPointData()->AddArray(labels);

  // Add priority array.
  vtkNew<vtkIntArray> sizes;
  sizes->SetNumberOfValues(6);
  sizes->SetName("sizes");
  sizes->SetValue(0, 10);
  sizes->SetValue(1, 7);
  sizes->SetValue(2, 6);
  sizes->SetValue(3, 4);
  sizes->SetValue(4, 4);
  sizes->SetValue(5, 4);
  pointSource->GetOutput()->GetPointData()->AddArray(sizes);

  // Create a mapper and actor for the points.
  vtkNew<vtkPolyDataMapper> pointMapper;
  pointMapper->SetInputConnection(pointSource->GetOutputPort());

  vtkNew<vtkActor> pointActor;
  pointActor->SetMapper(pointMapper);

  // Map the points to spheres
  auto sphereActor = PointToGlyph(pointSource->GetOutput()->GetPoints(), 0.05);
  sphereActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // Generate the label hierarchy.
  vtkNew<vtkPointSetToLabelHierarchy> pointSetToLabelHierarchyFilter;
  pointSetToLabelHierarchyFilter->SetInputConnection(
      pointSource->GetOutputPort());
  pointSetToLabelHierarchyFilter->SetLabelArrayName("labels");
  pointSetToLabelHierarchyFilter->SetPriorityArrayName("sizes");
  pointSetToLabelHierarchyFilter->Update();

  // Create a mapper and actor for the labels.
  vtkNew<vtkLabelPlacementMapper> labelMapper;
  labelMapper->SetInputConnection(
      pointSetToLabelHierarchyFilter->GetOutputPort());
  vtkNew<vtkActor2D> labelActor;
  labelActor->SetMapper(labelMapper);
  // labelActor->GetProperty()->SetColor(
  //    colors->GetColor3d("Yellow").GetData());

  // Create a renderer, render window, and interactor.
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("LabelPlacementMapper");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene.
  renderer->AddActor(pointActor);
  renderer->AddActor(sphereActor);
  renderer->AddActor(labelActor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Render and interact.
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
