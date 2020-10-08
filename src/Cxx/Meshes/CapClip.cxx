#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkClipPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkFeatureEdges.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkStripper.h>
#include <vtkXMLPolyDataReader.h>

// Readers
#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(std::string const& fileName);
}

//
// Demonstrate the use of clipping and capping on polyhedral data
//

int main(int argc, char* argv[])
{
  // Define colors
  vtkNew<vtkNamedColors> colors;
  auto backgroundColor = colors->GetColor3d("steel_blue");
  auto boundaryColor = colors->GetColor3d("banana");
  auto clipColor = colors->GetColor3d("tomato");
  // PolyData to process
  auto polyData = ReadPolyData(argc > 1 ? argv[1] : "");

  vtkNew<vtkPlane> plane;
  plane->SetOrigin(polyData->GetCenter());
  plane->SetNormal(1.0, -1.0, -1.0);

  vtkNew<vtkClipPolyData> clipper;
  clipper->SetInputData(polyData);
  clipper->SetClipFunction(plane);
  clipper->SetValue(0);
  clipper->Update();

  polyData = clipper->GetOutput();

  vtkNew<vtkDataSetMapper> clipMapper;
  clipMapper->SetInputData(polyData);

  vtkNew<vtkActor> clipActor;
  clipActor->SetMapper(clipMapper);
  clipActor->GetProperty()->SetDiffuseColor(clipColor.GetData());
  clipActor->GetProperty()->SetInterpolationToFlat();
  clipActor->GetProperty()->EdgeVisibilityOn();

  // Now extract feature edges
  vtkNew<vtkFeatureEdges> boundaryEdges;
  boundaryEdges->SetInputData(polyData);
  boundaryEdges->BoundaryEdgesOn();
  boundaryEdges->FeatureEdgesOff();
  boundaryEdges->NonManifoldEdgesOff();
  boundaryEdges->ManifoldEdgesOff();

  vtkNew<vtkStripper> boundaryStrips;
  boundaryStrips->SetInputConnection(boundaryEdges->GetOutputPort());
  boundaryStrips->Update();

  // Change the polylines into polygons
  vtkNew<vtkPolyData> boundaryPoly;
  boundaryPoly->SetPoints(boundaryStrips->GetOutput()->GetPoints());
  boundaryPoly->SetPolys(boundaryStrips->GetOutput()->GetLines());

  vtkNew<vtkPolyDataMapper> boundaryMapper;
  boundaryMapper->SetInputData(boundaryPoly);

  vtkNew<vtkActor> boundaryActor;
  boundaryActor->SetMapper(boundaryMapper);
  boundaryActor->GetProperty()->SetDiffuseColor(boundaryColor.GetData());

  // Create graphics stuff
  //
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(backgroundColor.GetData());
  renderer->UseHiddenLineRemovalOn();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer, set the background and size
  //
  renderer->AddActor(clipActor);
  renderer->AddActor(boundaryActor);

  // Generate an interesting view
  //
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.2);
  renderer->ResetCameraClippingRange();

  renderWindow->Render();
  renderWindow->SetWindowName("CapClip");
  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(std::string const& fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension = "";
  if (fileName.find_last_of(".") != std::string::npos)
  {
    extension = fileName.substr(fileName.find_last_of("."));
  }
  // Make the extension lowercase
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 ::tolower);
  if (extension == ".ply")
  {
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtp")
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".obj")
  {
    vtkNew<vtkOBJReader> reader;
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".stl")
  {
    vtkNew<vtkSTLReader> reader;
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtk")
  {
    vtkNew<vtkPolyDataReader> reader;
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".g")
  {
    vtkNew<vtkBYUReader> reader;
    reader->SetGeometryFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    // Return a polydata sphere if the extension is unknown.
    vtkNew<vtkSphereSource> source;
    source->SetThetaResolution(20);
    source->SetPhiResolution(11);
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}
} // namespace
