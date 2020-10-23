// test baking shadow maps

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCameraPass.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkLight.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGLTexture.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderPassCollection.h>
#include <vtkRenderStepsPass.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSequencePass.h>
#include <vtkShadowMapBakerPass.h>
#include <vtkShadowMapPass.h>
#include <vtkSmartPointer.h>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

#include <vtksys/SystemTools.hxx>

#include <array>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

//----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Read the polyData
  auto polyData = ReadPolyData(argc > 1 ? argv[1] : "");
  ;

  vtkNew<vtkNamedColors> colors;
  colors->SetColor("HighNoonSun", 1.0, 1.0, .9843, 1.0); // Color temp. 5400k
  colors->SetColor("100W Tungsten", 1.0, .8392, .6667,
                   1.0); // Color temp. 2850k

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 480);
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkLight> light1;
  light1->SetFocalPoint(0, 0, 0);
  light1->SetPosition(0, 1, 0.2);
  light1->SetColor(colors->GetColor3d("HighNoonSun").GetData());
  light1->SetIntensity(0.3);
  renderer->AddLight(light1);

  vtkNew<vtkLight> light2;
  light2->SetFocalPoint(0, 0, 0);
  light2->SetPosition(1.0, 1.0, 1.0);
  light2->SetColor(colors->GetColor3d("100W Tungsten").GetData());
  light2->SetIntensity(0.8);
  renderer->AddLight(light2);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polyData);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetAmbientColor(
      colors->GetColor3d("SaddleBrown").GetData());
  actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Sienna").GetData());
  actor->GetProperty()->SetSpecularColor(colors->GetColor3d("White").GetData());
  actor->GetProperty()->SetSpecular(0.51);
  actor->GetProperty()->SetDiffuse(0.7);
  actor->GetProperty()->SetAmbient(0.7);
  actor->GetProperty()->SetSpecularPower(30.0);
  actor->GetProperty()->SetOpacity(1.0);
  renderer->AddActor(actor);

  // add a plane
  std::array<double, 6> bounds;
  polyData->GetBounds(bounds.data());

  std::array<double, 3> range;
  range[0] = bounds[1] - bounds[0];
  range[1] = bounds[3] - bounds[2];
  range[2] = bounds[5] - bounds[4];
  std::cout << "range: " << range[0] << "," << range[1] << "," << range[2]
            << std::endl;
  double expand = 1.0;
  auto thickness = range[2] * 0.1;
  vtkNew<vtkCubeSource> plane;
  plane->SetCenter((bounds[1] + bounds[0]) / 2.0, bounds[2] - thickness / 2.0,
                   (bounds[5] + bounds[4]) / 2.0);
  plane->SetXLength(bounds[1] - bounds[0] + (range[0] * expand));
  plane->SetYLength(thickness);
  plane->SetZLength(bounds[5] - bounds[4] + (range[2] * expand));

  vtkNew<vtkPolyDataMapper> planeMapper;
  planeMapper->SetInputConnection(plane->GetOutputPort());

  vtkNew<vtkActor> planeActor;
  planeActor->SetMapper(planeMapper);
  renderer->AddActor(planeActor);

  renderWindow->SetMultiSamples(0);

  vtkNew<vtkShadowMapPass> shadows;

  vtkNew<vtkSequencePass> seq;

  vtkNew<vtkRenderPassCollection> passes;
  passes->AddItem(shadows->GetShadowMapBakerPass());
  passes->AddItem(shadows);
  seq->SetPasses(passes);

  vtkNew<vtkCameraPass> cameraP;
  cameraP->SetDelegatePass(seq);

  // tell the renderer to use our render pass pipeline
  vtkOpenGLRenderer* glrenderer =
      dynamic_cast<vtkOpenGLRenderer*>(renderer.GetPointer());
  glrenderer->SetPass(cameraP);

  renderer->GetActiveCamera()->SetPosition(-0.2, 0.2, 1);
  renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Dolly(2.25);
  renderer->ResetCameraClippingRange();
  renderWindow->Render();
  renderWindow->SetWindowName("Shadows");

  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension =
      vtksys::SystemTools::GetFilenameLastExtension(std::string(fileName));
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
    source->SetThetaResolution(100);
    source->SetPhiResolution(100);
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}
} // namespace
