#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkLegendBoxActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5000.0);
  sphereSource->Update();

  vtkPolyData* polydata = sphereSource->GetOutput();

  // Create a mapper
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polydata);

  // Create an actor
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Legend");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkLegendBoxActor> legend;
  legend->SetNumberOfEntries(2);

  double color[4];

  vtkNew<vtkCubeSource> legendBox;
  legendBox->Update();
  colors->GetColor("Tomato", color);
  legend->SetEntry(0, legendBox->GetOutput(), "Box", color);

  colors->GetColor("Banana", color);
  legend->SetEntry(1, sphereSource->GetOutput(), "Ball", color);

  // place legend in lower right
  legend->GetPositionCoordinate()->SetCoordinateSystemToView();
  legend->GetPositionCoordinate()->SetValue(0.5, -1.0);

  legend->GetPosition2Coordinate()->SetCoordinateSystemToView();
  legend->GetPosition2Coordinate()->SetValue(1.0, -0.5);

  legend->UseBackgroundOn();
  double background[4];
  colors->GetColor("SlateGray", background);
  legend->SetBackgroundColor(background);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->AddActor(legend);
  renderer->SetBackground(colors->GetColor3d("DarkGreen").GetData());

  // Render an image (lights and cameras are created automatically)
  renderWindow->Render();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
