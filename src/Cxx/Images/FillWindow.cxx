#include <vtkCamera.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageMapper3D.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an image
  vtkNew<vtkImageMandelbrotSource> mandelbrotSource;
  mandelbrotSource->Update();

  vtkNew<vtkImageCast> castFilter;
  castFilter->SetInputConnection(mandelbrotSource->GetOutputPort());
  castFilter->SetOutputScalarTypeToUnsignedChar();
  castFilter->Update();

  vtkNew<vtkImageSliceMapper> imageSliceMapper;
  imageSliceMapper->SetInputConnection(castFilter->GetOutputPort());

  vtkNew<vtkImageSlice> imageSlice;
  imageSlice->SetMapper(imageSliceMapper);

  int extent[6];
  castFilter->GetOutput()->GetExtent(extent);

  double origin[3];
  castFilter->GetOutput()->GetOrigin(origin);

  double spacing[3];
  castFilter->GetOutput()->GetSpacing(spacing);

  // Setup renderer
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(imageSlice);
  renderer->ResetCamera();

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("FillWindow");

  vtkCamera* camera = renderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  renderer->SetBackground(colors->GetColor3d("Red").GetData());

  float xc = origin[0] + 0.5 * (extent[0] + extent[1]) * spacing[0];
  float yc = origin[1] + 0.5 * (extent[2] + extent[3]) * spacing[1];
  //  float xd = (extent[1] - extent[0] + 1)*spacing[0]; // not used
  float yd = (extent[3] - extent[2] + 1) * spacing[1];

  float d = camera->GetDistance();
  camera->SetParallelScale(0.5f * static_cast<float>(yd));
  camera->SetFocalPoint(xc, yc, 0.0);
  camera->SetPosition(xc, yc, +d);

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
