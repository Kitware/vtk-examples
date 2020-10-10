#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkWindowedSincPolyDataFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkWindowedSincPolyDataFilter> smoother;
  smoother->SetInputConnection(sphereSource->GetOutputPort());
  smoother->SetNumberOfIterations(15);
  smoother->BoundarySmoothingOff();
  smoother->FeatureEdgeSmoothingOff();
  smoother->SetFeatureAngle(120.0);
  smoother->SetPassBand(.001);
  smoother->NonManifoldSmoothingOn();
  smoother->NormalizeCoordinatesOn();
  smoother->Update();

  vtkNew<vtkPolyDataMapper> smoothedMapper;
  smoothedMapper->SetInputConnection(smoother->GetOutputPort());
  vtkNew<vtkActor> smoothedActor;
  smoothedActor->SetMapper(smoothedMapper);
  smoothedActor->GetProperty()->SetColor(
      colors->GetColor3d("LightCoral").GetData());

  vtkNew<vtkPolyDataMapper> inputMapper;
  inputMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> inputActor;
  inputActor->SetMapper(inputMapper);
  inputActor->GetProperty()->SetColor(
      colors->GetColor3d("LightCoral").GetData());

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("WindowedSincPolyDataFilter");

  // And one interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("BurlyWood").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("DodgerBlue").GetData());

  // Add the sphere to the left and the smoothed sphere to the right
  leftRenderer->AddActor(inputActor);
  rightRenderer->AddActor(smoothedActor);

  leftRenderer->ResetCamera();

  rightRenderer->ResetCamera();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
