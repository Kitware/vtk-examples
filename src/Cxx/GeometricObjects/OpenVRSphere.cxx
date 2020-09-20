#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderer.h>
#include <vtkOpenVRRenderWindowInteractor.h>
#include <vtkSphereSource.h>


int main(int, char *[])
{
  // Create a Sphere
  vtkSmartPointer<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5.0);

  //Create a Mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  //Create an Actor
  vtkSmartPointer<vtkNamedColors> colors;
  vtkSmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);

  //Create a Renderer, RenderWindow, RenderWindowInteractor
  vtkSmartPointer<vtkOpenVRRenderer> renderer;
  vtkSmartPointer<vtkOpenVRRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("OpenVRSphere");
  vtkSmartPointer<vtkOpenVRRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  //Add actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("ForestGreen").GetData());
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
