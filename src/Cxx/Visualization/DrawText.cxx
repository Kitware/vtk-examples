#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5.0);
  sphereSource->Update();

  // Create a mapper
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(sphereSource->GetOutput());

  // Create an actor
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Create a renderer
  vtkNew<vtkRenderer> renderer;
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Create a render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("DrawText");

  // Create an interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Setup the text and add it to the renderer
  vtkNew<vtkTextActor> textActor;
  textActor->SetInput("Hello world");
  textActor->SetPosition2(10, 40);
  textActor->GetTextProperty()->SetFontSize(24);
  textActor->GetTextProperty()->SetColor(colors->GetColor3d("Gold").GetData());
  renderer->AddActor2D(textActor);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
