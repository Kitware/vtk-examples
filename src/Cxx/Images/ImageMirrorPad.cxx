#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMirrorPad.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkType.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an image
  vtkNew<vtkImageCanvasSource2D> source;
  source->SetExtent(0, 20, 0, 20, 0, 0);
  source->SetScalarTypeToUnsignedChar();
  source->SetDrawColor(0.0, 0.0, 0.0, 1.0);
  source->FillBox(-VTK_INT_MAX, VTK_INT_MAX, -VTK_INT_MAX, VTK_INT_MAX);
  source->SetDrawColor(255.0, 0.0, 0.0, 0.5);
  source->DrawCircle(10, 10, 5);
  source->Update();

  vtkNew<vtkImageMirrorPad> mirrorPadFilter;
  mirrorPadFilter->SetInputConnection(source->GetOutputPort());
  mirrorPadFilter->SetOutputWholeExtent(-10, 30, -10, 30, 0, 0);
  mirrorPadFilter->Update();

  // Create an actor
  vtkNew<vtkImageActor> actor;
  actor->GetMapper()->SetInputConnection(mirrorPadFilter->GetOutputPort());

  // Visualize
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("Salmon").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ImageMirrorPad");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
