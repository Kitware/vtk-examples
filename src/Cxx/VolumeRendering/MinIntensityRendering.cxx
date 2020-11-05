#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkImageClip.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Required arguments: vtkFile e.g. ironProt.vtk" << std::endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];

  vtkNew<vtkNamedColors> colors;

  // Create the renderers, render window, and interactor
  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);
  vtkNew<vtkRenderer> ren;
  renWin->AddRenderer(ren);
  renWin->SetWindowName("MinIntensityRendering");

  // Read the data from a vtk file
  vtkNew<vtkStructuredPointsReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();

  // Create a transfer function mapping scalar value to opacity
  vtkNew<vtkPiecewiseFunction> oTFun;
  oTFun->AddSegment(0, 1.0, 256, 0.1);

  vtkNew<vtkColorTransferFunction> cTFun;
  cTFun->AddRGBPoint(0, 1.0, 1.0, 1.0);
  cTFun->AddRGBPoint(255, 1.0, 1.0, 1.0);

  // Need to crop to actually see minimum intensity
  vtkNew<vtkImageClip> clip;
  clip->SetInputConnection(reader->GetOutputPort());
  clip->SetOutputWholeExtent(0, 66, 0, 66, 30, 37);
  clip->ClipDataOn();

  vtkNew<vtkVolumeProperty> property;
  property->SetScalarOpacity(oTFun);
  property->SetColor(cTFun);
  property->SetInterpolationTypeToLinear();

  vtkNew<vtkFixedPointVolumeRayCastMapper> mapper;
  mapper->SetBlendModeToMinimumIntensity();
  mapper->SetInputConnection(clip->GetOutputPort());

  vtkNew<vtkVolume> volume;
  volume->SetMapper(mapper);
  volume->SetProperty(property);

  ren->AddViewProp(volume);
  ren->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  renWin->Render();

  ren->GetActiveCamera()->Zoom(1.3);

  iren->Start();

  return EXIT_SUCCESS;
}
