#include <vtkActor.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRTAnalyticSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRTAnalyticSource> analyticSource;
  analyticSource->SetWholeExtent(-10, 10, -10, 10, 0, 0);

  vtkNew<vtkImageActor> imageActor;
  imageActor->GetMapper()->SetInputConnection(analyticSource->GetOutputPort());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("RTAnalyticSource");

  vtkNew<vtkRenderWindowInteractor> interactor;

  vtkNew<vtkInteractorStyleImage> style;
  interactor->SetInteractorStyle(style);

  interactor->SetRenderWindow(renderWindow);

  // Setup both renderers
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("Wheat").GetData());
  renderWindow->AddRenderer(renderer);

  renderer->AddActor(imageActor);

  renderer->ResetCamera();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
