#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourValues.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMetaImageReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOpenGLGPUVolumeRayCastMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>

int main(int argc, char* argv[])
{
  double iso1 = 500.0;
  double iso2 = 1150.0;

  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.mnd [iso1=500] [iso2=1150]"
              << std::endl;
    std::cout << "e.g. FullHead.mhd 500 1150" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkMetaImageReader> reader;
  reader->SetFileName(argv[1]);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkOpenGLGPUVolumeRayCastMapper> mapper;
  mapper->SetInputConnection(reader->GetOutputPort());
  mapper->AutoAdjustSampleDistancesOff();
  mapper->SetSampleDistance(0.5);
  mapper->SetBlendModeToIsoSurface();

  if (argc > 3)
  {
    iso1 = atof(argv[2]);
    iso2 = atof(argv[3]);
  }
  vtkNew<vtkColorTransferFunction> colorTransferFunction;
  colorTransferFunction->RemoveAllPoints();
  colorTransferFunction->AddRGBPoint(iso2,
                                     colors->GetColor3d("ivory").GetData()[0],
                                     colors->GetColor3d("ivory").GetData()[1],
                                     colors->GetColor3d("ivory").GetData()[2]);
  colorTransferFunction->AddRGBPoint(iso1,
                                     colors->GetColor3d("flesh").GetData()[0],
                                     colors->GetColor3d("flesh").GetData()[1],
                                     colors->GetColor3d("flesh").GetData()[2]);

  vtkNew<vtkPiecewiseFunction> scalarOpacity;
  scalarOpacity->AddPoint(iso1, .3);
  scalarOpacity->AddPoint(iso2, 0.6);

  vtkNew<vtkVolumeProperty> volumeProperty;
  volumeProperty->ShadeOn();
  volumeProperty->SetInterpolationTypeToLinear();
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(scalarOpacity);

  vtkNew<vtkVolume> volume;
  volume->SetMapper(mapper);
  volume->SetProperty(volumeProperty);

  vtkNew<vtkRenderer> renderer;
  renderer->AddVolume(volume);
  renderer->SetBackground(colors->GetColor3d("cornflower").GetData());
  renderer->ResetCamera();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(800, 600);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("RayCastIsosurface");

  vtkNew<vtkInteractorStyleTrackballCamera> style;

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);
  interactor->SetInteractorStyle(style);

  // Add some contour values to draw iso surfaces
  volumeProperty->GetIsoSurfaceValues()->SetValue(0, iso1);
  volumeProperty->GetIsoSurfaceValues()->SetValue(1, iso2);

  // Generate a good view
  vtkNew<vtkCamera> aCamera;
  aCamera->SetViewUp(0, 0, -1);
  aCamera->SetPosition(0, -1, 0);
  aCamera->SetFocalPoint(0, 0, 0);

  renderer->SetActiveCamera(aCamera);
  renderer->ResetCamera();

  aCamera->Azimuth(30.0);
  aCamera->Elevation(30.0);
  aCamera->Dolly(1.5);
  renderer->ResetCameraClippingRange();

  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
}
