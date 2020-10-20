#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an arrow.
  vtkNew<vtkArrowSource> arrowSource;

  vtkNew<vtkTransform> transform;
  transform->Scale(5, 1, 1);

  vtkNew<vtkTransformFilter> transformFilter;
  transformFilter->SetInputConnection(arrowSource->GetOutputPort());
  transformFilter->SetTransform(transform);

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(transformFilter->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Coral").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("TransformFilter");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("CadetBlue").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
