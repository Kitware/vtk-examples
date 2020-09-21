#include <vtkActor.h>
#include <vtkEarthSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGWriter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkWindowToImageFilter.h>

int main(int, char*[])
{

  vtkNew<vtkNamedColors> colors;

  // Earth source
  vtkNew<vtkEarthSource> earthSource;
  earthSource->OutlineOn();
  earthSource->Update();

  // Create a sphere
  vtkNew<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(earthSource->GetRadius());

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(earthSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphere->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(
      colors->GetColor3d("PeachPuff").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->AddActor(sphereActor);
  renderer->SetBackground(colors->GetColor3d("Black").GetData());

  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("EarthSource");

  // Render and interact
  renderWindow->Render();

  /*
  // screenshot code:
  vtkNew<vtkWindowToImageFilter> w2if ;
  w2if->SetInput(renderWindow);
  w2if->Update();

  vtkNew<vtkPNGWriter> writer;
  writer->SetFileName("TestEarthSource->png");
  writer->SetInputConnection(w2if->GetOutputPort());
  writer->Write();
  */

  // begin interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
