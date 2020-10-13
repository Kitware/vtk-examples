#include <vtkActor.h>
#include <vtkBYUReader.h>
#include <vtkCamera.h>
#include <vtkExtractEnclosedPoints.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

#include <vtksys/SystemTools.hxx>

#include <random>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

int main(int argc, char* argv[])
{
  auto polyData = ReadPolyData(argc > 1 ? argv[1] : "");
  std::mt19937 mt(4355412); // Standard mersenne_twister_engine
  double bounds[6];
  polyData->GetBounds(bounds);
  std::cout << "Bounds: " << bounds[0] << ", " << bounds[1] << " " << bounds[2]
            << ", " << bounds[3] << " " << bounds[4] << ", " << bounds[5]
            << std::endl;
  // Generate random points within the bounding box of the polydata
  std::uniform_real_distribution<double> distributionX(bounds[0], bounds[1]);
  std::uniform_real_distribution<double> distributionY(bounds[2], bounds[3]);
  std::uniform_real_distribution<double> distributionZ(bounds[4], bounds[5]);
  vtkNew<vtkPolyData> pointsPolyData;
  vtkNew<vtkPoints> points;
  pointsPolyData->SetPoints(points);

  points->SetNumberOfPoints(10000);
  for (auto i = 0; i < 10000; ++i)
  {
    double point[3];
    point[0] = distributionX(mt);
    point[1] = distributionY(mt);
    point[2] = distributionZ(mt);
    points->SetPoint(i, point);
  }

  vtkNew<vtkExtractEnclosedPoints> extract;
  extract->SetSurfaceData(polyData);
  extract->SetInputData(pointsPolyData);
  extract->SetTolerance(.0001);
  extract->CheckSurfaceOn();

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> source;
  source->SetRadius((bounds[1] - bounds[0]) * .005);

  vtkNew<vtkGlyph3DMapper> glyph3Dmapper;
  glyph3Dmapper->SetSourceConnection(source->GetOutputPort());
  glyph3Dmapper->SetInputConnection(extract->GetOutputPort());

  vtkNew<vtkActor> glyphActor;
  glyphActor->SetMapper(glyph3Dmapper);
  glyphActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Crimson").GetData());
  glyphActor->GetProperty()->SetSpecular(0.6);
  glyphActor->GetProperty()->SetSpecularPower(30);
  ;

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polyData);

  vtkNew<vtkProperty> backProp;
  backProp->SetDiffuseColor(colors->GetColor3d("Banana").GetData());
  backProp->SetSpecular(0.6);
  backProp->SetSpecularPower(30);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->SetBackfaceProperty(backProp);
  actor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Crimson").GetData());
  actor->GetProperty()->SetSpecular(0.6);
  actor->GetProperty()->SetSpecularPower(30);
  actor->GetProperty()->SetOpacity(0.3);

  vtkNew<vtkRenderer> renderer;
  renderer->UseHiddenLineRemovalOn();
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->AddActor(glyphActor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());
  renderer->UseHiddenLineRemovalOn();

  renderWindow->SetSize(640, 512);
  renderWindow->SetWindowName("ExtractEnclosedPoints");

  renderWindow->Render();
  renderWindowInteractor->Start();

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
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}
} // namespace
