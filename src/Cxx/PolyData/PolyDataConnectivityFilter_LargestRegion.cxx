#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Small sphere
  vtkNew<vtkSphereSource> sphereSource1;
  sphereSource1->SetRadius(5);

  // Large sphere
  vtkNew<vtkSphereSource> sphereSource2;
  sphereSource2->SetRadius(10);
  sphereSource2->SetCenter(25, 0, 0);
  sphereSource2->SetThetaResolution(10);
  sphereSource2->SetPhiResolution(10);

  vtkNew<vtkAppendPolyData> appendFilter;
  appendFilter->AddInputConnection(sphereSource1->GetOutputPort());
  appendFilter->AddInputConnection(sphereSource2->GetOutputPort());

  vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter;
  connectivityFilter->SetInputConnection(appendFilter->GetOutputPort());
  connectivityFilter->SetExtractionModeToLargestRegion();

  // Create a mapper and actor for original data
  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputConnection(appendFilter->GetOutputPort());

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());

  // Create a mapper and actor for extracted data
  vtkNew<vtkPolyDataMapper> extractedMapper;
  extractedMapper->SetInputConnection(connectivityFilter->GetOutputPort());
  extractedMapper->Update();

  vtkNew<vtkActor> extractedActor;
  extractedActor->GetProperty()->SetColor(
      colors->GetColor3d("Banana").GetData());
  extractedActor->SetMapper(extractedMapper);
  extractedActor->SetPosition(0, -20, 0);

  // Visualization
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(originalActor);
  renderer->AddActor(extractedActor);
  renderer->GradientBackgroundOn();
  renderer->SetBackground(colors->GetColor3d("Gold").GetData());
  renderer->SetBackground2(colors->GetColor3d("Silver").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("PolyDataConnectivityFilter_LargestRegion");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
