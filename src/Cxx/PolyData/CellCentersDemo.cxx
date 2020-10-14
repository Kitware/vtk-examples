#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellCenters.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>
#include <vtksys/SystemTools.hxx>

#include <vtkColor.h>
#include <vtkNamedColors.h>
#include <vtkSphereSource.h>

#include <algorithm>
#include <vector>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

int main(int argc, char* argv[])
{
  // Define the colors used in the example
  vtkNew<vtkNamedColors> colors;

  vtkColor3d actorColor = colors->GetColor3d("Banana");
  vtkColor3d pointActorColor = colors->GetColor3d("Peacock");
  vtkColor3d backgroundColor = colors->GetColor3d("Silver");

  // Read the polydata
  auto polyData = ReadPolyData(argc > 1 ? argv[1] : "");

  std::vector<double> bounds(6);
  polyData->GetBounds(&bounds[0]);
  std::vector<double> range(3);
  for (int i = 0; i < 3; ++i)
  {
    range[i] = bounds[2 * i + 1] - bounds[2 * i];
  }
  std::cout << "Range: " << range[0] << ", " << range[1] << ", " << range[2]
            << std::endl;

  vtkNew<vtkCellCenters> centers;
  centers->SetInputData(polyData);

  auto maxRange = std::max_element(range.begin(), range.end());
  double radius = 0;
  if (argc > 1)
  {
    radius = *maxRange * 0.0025;
  }
  else
  {
    radius = *maxRange * 0.01;
  }
  vtkNew<vtkSphereSource> sphere;
  sphere->SetPhiResolution(11);
  sphere->SetThetaResolution(11);
  sphere->SetRadius(radius);

  vtkNew<vtkGlyph3DMapper> pointMapper;
  pointMapper->SetInputConnection(centers->GetOutputPort());
  pointMapper->SetSourceConnection(sphere->GetOutputPort());

  vtkNew<vtkActor> pointActor;
  pointActor->SetMapper(pointMapper);
  pointActor->GetProperty()->SetColor(pointActorColor.GetData());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polyData);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(actorColor.GetData());
  actor->GetProperty()->EdgeVisibilityOn();
  actor->GetProperty()->SetInterpolationToFlat();

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->AddActor(actor);
  renderer->AddActor(pointActor);

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.5);
  renderer->ResetCameraClippingRange();
  renderer->SetBackground(backgroundColor.GetData());

  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("CellCentersDemo");
  renderWindow->Render();

  renderWindowInteractor->Start();

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
