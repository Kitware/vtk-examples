#include <vtkClipPolyData.h>
#include <vtkFrustumSource.h>

#include <vtkNamedColors.h>
#include <vtkPlanes.h>
#include <vtkPolyDataNormals.h>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

#include <vtksys/SystemTools.hxx>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkMapper.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
void PositionCamera(vtkRenderer* renderer, double* viewUp,
                    double* position);
} // namespace

int main(int argc, char* argv[])
{
  auto polyData = ReadPolyData(argc > 1 ? argv[1] : "");

  vtkNew<vtkNamedColors> colors;

  // a renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  // an interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polyData);
  mapper->ScalarVisibilityOff();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Crimson").GetData());
  actor->GetProperty()->SetSpecular(.6);
  actor->GetProperty()->SetSpecularPower(30);
  renderer->AddActor(actor);

  vtkNew<vtkPolyDataMapper> outMapper;
  outMapper->SetInputData(polyData);
  outMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> outActor;
  outActor->SetMapper(outMapper);
  outActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Gold").GetData());
  outActor->GetProperty()->SetSpecular(.6);
  outActor->GetProperty()->SetSpecularPower(30);

  // Position the camera so that we can see the frustum
  double viewUp[3] = {0.0, 1.0, 0.0};
  double position[3] = {1.0, 0.0, 0.0};
  PositionCamera(renderer, viewUp, position);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->SetViewAngle(10.0);

  double planesArray[24];
  renderer->GetActiveCamera()->GetFrustumPlanes(1.0, planesArray);

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
  frustumActor->GetProperty()->SetOpacity(.5);
  frustumActor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());

  vtkNew<vtkClipPolyData> clip;
  clip->SetInputData(polyData);
  clip->SetClipFunction(planes);
  clip->InsideOutOn();
  clip->GenerateClippedOutputOn();
  clip->Update();

  mapper->SetInputConnection(clip->GetOutputPort());
  outMapper->SetInputData(clip->GetClippedOutput());

  renderer->SetBackground(colors->GetColor3d("Silver").GetData());
  renderer->AddActor(frustumActor);
  renderer->AddActor(outActor);

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.5);
  renderer->ResetCameraClippingRange();

  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("ClipFrustum");

  renderWindow->Render();

  // begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

// Snippets
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

void PositionCamera(vtkRenderer* renderer, double* viewUp, double* position)
{
  renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
  renderer->GetActiveCamera()->SetViewUp(viewUp);
  renderer->GetActiveCamera()->SetPosition(position);
  renderer->ResetCamera();
  return;
}
} // namespace
