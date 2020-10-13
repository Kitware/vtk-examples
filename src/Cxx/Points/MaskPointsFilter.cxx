#include <vtkBoundedPointSource.h>
#include <vtkCamera.h>
#include <vtkCone.h>
#include <vtkGlyph3D.h>
#include <vtkImageData.h>
#include <vtkImageThreshold.h>
#include <vtkMaskPointsFilter.h>
#include <vtkMetaImageReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleImplicitFunctionFilter.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace {
vtkSmartPointer<vtkImageData> CreatePoints();
}

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkSmartPointer<vtkImageData> imageMask;
  if (argc < 2)
  {
    imageMask = CreatePoints();
  }
  else
  {
    int upper = 1100;
    if (argc > 2)
    {
      upper = atoi(argv[2]);
    }
    // Read the volume data e.g. FullHead.mhd
    vtkNew<vtkMetaImageReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();

    vtkNew<vtkImageThreshold> threshold;
    threshold->SetInputConnection(reader->GetOutputPort());
    threshold->ThresholdByUpper(upper);
    threshold->SetOutputScalarTypeToUnsignedChar();
    threshold->ReplaceInOn();
    threshold->SetInValue(255);
    threshold->SetOutValue(0);
    threshold->ReplaceOutOn();
    threshold->Update();
    imageMask = dynamic_cast<vtkImageData*>(threshold->GetOutput());
  }
  vtkNew<vtkBoundedPointSource> pointSource;
  pointSource->SetNumberOfPoints(100000);
  pointSource->SetBounds(imageMask->GetBounds());

  vtkNew<vtkMaskPointsFilter> maskPoints;
  maskPoints->SetInputConnection(pointSource->GetOutputPort());
  maskPoints->SetMaskData(imageMask);

  double radius = imageMask->GetSpacing()[0] * 4.0;
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(radius);

  vtkNew<vtkGlyph3D> glyph;
  glyph->SetInputConnection(maskPoints->GetOutputPort());
  glyph->SetSourceConnection(sphereSource->GetOutputPort());
  glyph->ScalingOff();
  glyph->Update();

  vtkNew<vtkPolyDataMapper> glyphMapper;
  glyphMapper->SetInputConnection(glyph->GetOutputPort());
  glyphMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> glyphActor;
  glyphActor->SetMapper(glyphMapper);
  glyphActor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());

  // Create graphics stuff
  //
  vtkNew<vtkRenderer> ren1;
  ren1->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetSize(512, 512);
  renWin->SetWindowName("MaskPointsFilter");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(glyphActor);

  // Generate an interesting view
  //
  ren1->GetActiveCamera()->SetPosition(1, 0, 0);
  ren1->GetActiveCamera()->SetFocalPoint(0, 1, 0);
  ren1->GetActiveCamera()->SetViewUp(0, 0, -1);
  ren1->GetActiveCamera()->Azimuth(30);
  ren1->GetActiveCamera()->Elevation(30);
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Dolly(1.0);
  ren1->ResetCameraClippingRange();

  renWin->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkImageData> CreatePoints()
{
  vtkSmartPointer<vtkImageData> imageMask;

  vtkNew<vtkImageData> image;
  image->SetDimensions(256, 256, 256);
  image->AllocateScalars(VTK_DOUBLE, 1);
  image->SetSpacing(5.0 / 255.0, 5.0 / 255.0, 5.0 / 255.0);
  image->SetOrigin(-2.5, -2.5, -2.5);
  vtkNew<vtkCone> implicitFunction;
  vtkNew<vtkSampleImplicitFunctionFilter> sample;
  sample->SetImplicitFunction(implicitFunction);
  sample->SetInputData(image);

  vtkNew<vtkImageThreshold> threshold;
  threshold->SetInputConnection(sample->GetOutputPort());
  threshold->ThresholdByLower(.5);
  threshold->SetOutputScalarTypeToUnsignedChar();
  threshold->ReplaceInOn();
  threshold->SetInValue(255);
  threshold->SetOutValue(0);
  threshold->ReplaceOutOn();
  threshold->Update();
  imageMask = dynamic_cast<vtkImageData*>(threshold->GetOutput());
  return imageMask;
}
} // namespace
