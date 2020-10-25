#include <vtkActor.h>
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
#include <vtkVersion.h>
#include <vtkWindowToImageFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5.0);
  sphereSource->SetPhiResolution(30);
  sphereSource->SetThetaResolution(30);
  sphereSource->Update();

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("IndianRed").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetAlphaBitPlanes(1); // enable usage of alpha channel
  renderWindow->SetWindowName("Screenshot");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MistyRose").GetData());

  renderWindow->Render();

  // Screenshot
  vtkNew<vtkWindowToImageFilter> windowToImageFilter;
  windowToImageFilter->SetInput(renderWindow);
#if VTK_MAJOR_VERSION >= 8 || VTK_MAJOR_VERSION == 8 && VTK_MINOR_VERSION >= 90
  windowToImageFilter->SetScale(2); // image quality
#else
  windowToImageFilter->SetMagnification(2); // image quality
#endif
  windowToImageFilter->SetInputBufferTypeToRGBA(); // also record the alpha
                                                   // (transparency) channel
  windowToImageFilter->ReadFrontBufferOff();       // read from the back buffer
  windowToImageFilter->Update();

  vtkNew<vtkPNGWriter> writer;
  writer->SetFileName("screenshot2.png");
  writer->SetInputConnection(windowToImageFilter->GetOutputPort());
  writer->Write();

  renderWindow->Render();
  renderer->ResetCamera();
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
