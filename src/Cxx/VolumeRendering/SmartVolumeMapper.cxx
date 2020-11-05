#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPointData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkSphere.h>
#include <vtkVolumeProperty.h>
#include <vtkXMLImageDataReader.h>

namespace {
void CreateImageData(vtkImageData* im);
}

int main(int argc, char* argv[])
{
  vtkNew<vtkImageData> imageData;
  if (argc < 2)
  {
    CreateImageData(imageData);
  }
  else
  {
    vtkNew<vtkXMLImageDataReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();
    imageData->ShallowCopy(reader->GetOutput());
  }

  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderer> ren1;
  ren1->SetBackground(0.1, 0.4, 0.2);

  renWin->AddRenderer(ren1);

  renWin->SetSize(301, 300); // intentional odd and NPOT  width/height

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  renWin->Render(); // make sure we have an OpenGL context.

  vtkNew<vtkSmartVolumeMapper> volumeMapper;
  volumeMapper->SetBlendModeToComposite(); // composite first
  volumeMapper->SetInputData(imageData);

  vtkNew<vtkVolumeProperty> volumeProperty;
  volumeProperty->ShadeOff();
  volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

  vtkNew<vtkPiecewiseFunction> compositeOpacity;
  compositeOpacity->AddPoint(0.0, 0.0);
  compositeOpacity->AddPoint(80.0, 1.0);
  compositeOpacity->AddPoint(80.1, 0.0);
  compositeOpacity->AddPoint(255.0, 0.0);
  volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.

  vtkNew<vtkColorTransferFunction> color;
  color->AddRGBPoint(0.0, 0.0, 0.0, 1.0);
  color->AddRGBPoint(40.0, 1.0, 0.0, 0.0);
  color->AddRGBPoint(255.0, 1.0, 1.0, 1.0);
  volumeProperty->SetColor(color);

  vtkNew<vtkVolume> volume;
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  ren1->AddViewProp(volume);
  ren1->ResetCamera();

  // Render composite. In default mode. For coverage.
  renWin->Render();

  // 3D texture mode. For coverage.
#if !defined(VTK_LEGACY_REMOVE) && !defined(VTK_OPENGL2)
  volumeMapper->SetRequestedRenderModeToRayCastAndTexture();
#endif // VTK_LEGACY_REMOVE
  renWin->Render();

  // Software mode, for coverage. It also makes sure we will get the same
  // regression image on all platforms.
  volumeMapper->SetRequestedRenderModeToRayCast();
  renWin->Render();

  iren->Start();

  return EXIT_SUCCESS;
}

namespace {
void CreateImageData(vtkImageData* imageData)
{
  // Create a spherical implicit function.
  vtkNew<vtkSphere> sphere;
  sphere->SetRadius(0.1);
  sphere->SetCenter(0.0, 0.0, 0.0);

  vtkNew<vtkSampleFunction> sampleFunction;
  sampleFunction->SetImplicitFunction(sphere);
  sampleFunction->SetOutputScalarTypeToDouble();
  sampleFunction->SetSampleDimensions(127, 127,
                                      127); // intentional NPOT dimensions.
  sampleFunction->SetModelBounds(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
  sampleFunction->SetCapping(false);
  sampleFunction->SetComputeNormals(false);
  sampleFunction->SetScalarArrayName("values");
  sampleFunction->Update();

  vtkDataArray* a =
      sampleFunction->GetOutput()->GetPointData()->GetScalars("values");
  double range[2];
  a->GetRange(range);

  vtkNew<vtkImageShiftScale> t;
  t->SetInputConnection(sampleFunction->GetOutputPort());

  t->SetShift(-range[0]);
  double magnitude = range[1] - range[0];
  if (magnitude == 0.0)
  {
    magnitude = 1.0;
  }
  t->SetScale(255.0 / magnitude);
  t->SetOutputScalarTypeToUnsignedChar();

  t->Update();

  imageData->ShallowCopy(t->GetOutput());
}
} // namespace
