#include <vtkActor.h>
#include <vtkContourFilter.h>
#include <vtkGaussianSplatter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataWriter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create points on a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(sphereSource->GetOutput()->GetPoints());

  vtkNew<vtkGaussianSplatter> splatter;
  splatter->SetInputData(polydata);
  splatter->SetSampleDimensions(50, 50, 50);
  splatter->SetRadius(0.5);
  splatter->ScalarWarpingOff();

  vtkNew<vtkContourFilter> surface;
  surface->SetInputConnection(splatter->GetOutputPort());
  surface->SetValue(0, 0.01);

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(surface->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  renderWindow->SetWindowName("GaussianSplat");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
