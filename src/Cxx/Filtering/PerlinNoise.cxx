#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPerlinNoise.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPerlinNoise> perlinNoise;
  perlinNoise->SetFrequency(2, 1.25, 1.5);
  perlinNoise->SetPhase(0, 0, 0);

  vtkNew<vtkSampleFunction> sample;
  sample->SetImplicitFunction(perlinNoise);
  sample->SetSampleDimensions(65, 65, 20);
  sample->ComputeNormalsOff();

  vtkNew<vtkContourFilter> surface;
  surface->SetInputConnection(sample->GetOutputPort());
  surface->SetValue(0, 0.0);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(surface->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer, set the background and size

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderWindow->SetWindowName("PerlinNoise");
  renderWindow->SetSize(300, 300);
  renderer->ResetCamera();
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
