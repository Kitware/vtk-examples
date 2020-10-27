#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkClipPolyData.h>
#include <vtkDecimatePro.h>
#include <vtkImageConstantPad.h>
#include <vtkImageData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageRGBToHSV.h>
#include <vtkImageSeedConnectivity.h>
#include <vtkImageShrink3D.h>
#include <vtkImageThreshold.h>
#include <vtkJPEGReader.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTriangleFilter.h>

#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename(.jpg)" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkJPEGReader> imageIn;
  imageIn->SetFileName(argv[1]);

  // Convert the image to hsv so that we can threshold on value
  vtkNew<vtkImageRGBToHSV> toHSV;
  toHSV->SetInputConnection(imageIn->GetOutputPort());

  // Just work with the value
  vtkNew<vtkImageExtractComponents> extractImage;
  extractImage->SetInputConnection(toHSV->GetOutputPort());
  extractImage->SetComponents(2);

  // Threshold to a black/white image
  vtkNew<vtkImageThreshold> threshold1;
  threshold1->SetInputConnection(extractImage->GetOutputPort());
  threshold1->ThresholdByUpper(230);
  threshold1->SetInValue(255);
  threshold1->SetOutValue(0);
  threshold1->Update();

  // Place a seed in each corner and label connected pixels with 255
  threshold1->UpdateInformation();

  int* extent = threshold1->GetOutputInformation(0)->Get(
      vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
  vtkNew<vtkImageSeedConnectivity> connect;
  connect->SetInputConnection(threshold1->GetOutputPort());
  connect->SetInputConnectValue(255);
  connect->SetOutputConnectedValue(255);
  connect->SetOutputUnconnectedValue(0);
  connect->AddSeed(extent[0], extent[2]);
  connect->AddSeed(extent[1], extent[2]);
  connect->AddSeed(extent[1], extent[3]);
  connect->AddSeed(extent[0], extent[3]);

  // Smooth a little before clipping
  vtkNew<vtkImageGaussianSmooth> smooth;
  smooth->SetDimensionality(2);
  smooth->SetStandardDeviation(1.0, 1.0);
  smooth->SetInputConnection(connect->GetOutputPort());

  vtkNew<vtkImageShrink3D> shrink;
  shrink->SetInputConnection(smooth->GetOutputPort());
  shrink->SetShrinkFactors(1, 1, 1);
  shrink->AveragingOn();

  // Convert the image to polydata
  vtkNew<vtkImageDataGeometryFilter> geometry;
  geometry->SetInputConnection(shrink->GetOutputPort());

  // Create texture coordinates
  vtkNew<vtkTextureMapToPlane> geometryTexture;
  geometryTexture->SetInputConnection(geometry->GetOutputPort());
  geometryTexture->SetOrigin(0, 0, 0);
  geometryTexture->SetPoint1(extent[1], 0, 0);
  geometryTexture->SetPoint2(0, extent[3], 0);

  // Clip the geometry
  vtkNew<vtkClipPolyData> clip;
  clip->SetInputConnection(geometryTexture->GetOutputPort());
  clip->SetValue(5.5);
  clip->GenerateClipScalarsOff();
  clip->InsideOutOff();
  clip->InsideOutOn();

  vtkNew<vtkTriangleFilter> triangles;
  triangles->SetInputConnection(clip->GetOutputPort());

  vtkNew<vtkDecimatePro> decimate;
  decimate->SetInputConnection(triangles->GetOutputPort());
  decimate->BoundaryVertexDeletionOn();
  decimate->SetDegree(100);
  decimate->PreserveTopologyOn();
  decimate->SetTargetReduction(.99);

  vtkNew<vtkLinearExtrusionFilter> extrude;
  extrude->SetInputConnection(decimate->GetOutputPort());
  extrude->SetExtrusionTypeToNormalExtrusion();
  extrude->SetScaleFactor(-10);

  vtkNew<vtkPolyDataMapper> map;
  map->SetInputConnection(extrude->GetOutputPort());
  map->ScalarVisibilityOff();

  vtkNew<vtkTexture> imageTexture;
  imageTexture->InterpolateOn();
  imageTexture->SetInputConnection(imageIn->GetOutputPort());

  vtkNew<vtkActor> clipart;
  clipart->SetMapper(map);
  clipart->SetTexture(imageTexture);

  // Create the RenderWindow, Renderer and Interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ClipArt");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(clipart);

  // Create a nice view
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.5);
  renderer->ResetCameraClippingRange();

  renderer->SetBackground(0.2, 0.3, 0.4);
  renderWindow->SetSize(320, 256);

  interactor->Initialize();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
