#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkShrinkPolyData.h>
#include <vtkNew.h>

int main(int , char *[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a pentagon
  vtkNew<vtkRegularPolygonSource> polygonSource;
  polygonSource->SetNumberOfSides(5);
  polygonSource->SetRadius(5);
  polygonSource->SetCenter(0, 0, 0);

  vtkNew<vtkShrinkPolyData> shrink;
  shrink->SetInputConnection(polygonSource->GetOutputPort());
  shrink->SetShrinkFactor(0.9);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(shrink->GetOutputPort());

  vtkNew<vtkProperty> back;
  back->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->EdgeVisibilityOn();
  actor->GetProperty()->SetLineWidth(5);
  actor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
  actor->SetBackfaceProperty(back);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("RegularPolygonSource");
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();
 
  return EXIT_SUCCESS;
}
