#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkFrustumSource.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProjectedTexture.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

#include <vtksys/SystemTools.hxx>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " polydata texture" << std::endl;
    return EXIT_FAILURE;
  }
  vtkSmartPointer<vtkPolyData> polyData = ReadPolyData(argc > 1 ? argv[1] : "");
  ;

  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> textureFile;
  textureFile.TakeReference(readerFactory->CreateImageReader2(argv[2]));
  textureFile->SetFileName(argv[2]);
  textureFile->Update();

  vtkNew<vtkNamedColors> colors;

  double center[3];
  polyData->GetCenter(center);
  std::cout << "Center: " << center[0] << ", " << center[1] << ", " << center[2]
            << std::endl;
  vtkNew<vtkCamera> camera;
  camera->SetPosition(center[0], center[1] + .0375, center[2] + .5);
  camera->SetFocalPoint(center[0], center[1] + .0375, center[2]);
  camera->Azimuth(-45);
  camera->Roll(-90);
  camera->SetClippingRange(.5, .6);
  double planesArray[24];

  camera->GetFrustumPlanes(1.0, planesArray);

  // Settings for vtkProjectedTexture

  double aspect[3];
  aspect[1] = 1.0;
  aspect[2] = 1.0;
  aspect[0] = 1.0;
  //(1.0/ (vtkMath::RadiansFromDegrees(std::tan(camera->GetViewAngle()))))
  /// 2.0;
  std::cout << "Aspect: " << aspect[0] << ", " << aspect[1] << ", " << aspect[2]
            << std::endl;

  vtkNew<vtkProjectedTexture> projectedTexture;

  projectedTexture->SetAspectRatio(aspect);
  projectedTexture->SetPosition(camera->GetPosition());
  projectedTexture->SetFocalPoint(camera->GetFocalPoint());
  projectedTexture->SetUp(camera->GetViewUp()[0], camera->GetViewUp()[1],
                          camera->GetViewUp()[2]);

  projectedTexture->SetInputData(
      polyData); // this can be the same as the one to project on
  projectedTexture->Update();

  // Map Texture on Surface
  polyData->GetPointData()->SetTCoords(
      projectedTexture->GetOutput()->GetPointData()->GetTCoords());

  vtkNew<vtkTexture> texture;
  texture->SetInputData(textureFile->GetOutput());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polyData);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->SetTexture(texture);

  vtkNew<vtkPlanes> planes;
  planes->SetFrustumPlanes(planesArray);

  vtkNew<vtkFrustumSource> frustumSource;
  frustumSource->ShowLinesOff();
  frustumSource->SetPlanes(planes);
  frustumSource->Update();

  vtkNew<vtkPolyDataMapper> frustumMapper;
  frustumMapper->SetInputConnection(frustumSource->GetOutputPort());

  vtkNew<vtkActor> frustumActor;
  frustumActor->SetMapper(frustumMapper);
  frustumActor->GetProperty()->EdgeVisibilityOn();
  frustumActor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
  frustumActor->GetProperty()->SetOpacity(.5);
  frustumActor->GetProperty()->SetRepresentationToWireframe();

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->AddActor(frustumActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->SetActiveCamera(camera);
  camera->Azimuth(180);
  camera->Roll(-90);
  renderer->ResetCameraClippingRange();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ProjectedTexture");

  vtkNew<vtkRenderWindowInteractor> renWinInteractor;
  renWinInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renWinInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension =
      vtksys::SystemTools::GetFilenameExtension(std::string(fileName));
  if (extension == ".ply")
  {
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtp")
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".obj")
  {
    vtkNew<vtkOBJReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".stl")
  {
    vtkNew<vtkSTLReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtk")
  {
    vtkNew<vtkPolyDataReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".g")
  {
    vtkNew<vtkBYUReader> reader;
    reader->SetGeometryFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkSphereSource> source;
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}
} // namespace
