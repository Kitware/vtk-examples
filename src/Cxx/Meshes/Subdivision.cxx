#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTriangle.h>
#include <vtkTriangleFilter.h>
#include <vtkXMLPolyDataReader.h>

#include <vtkButterflySubdivisionFilter.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkLoopSubdivisionFilter.h>

// Readers
#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

#include <map>
#include <string>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(std::string const& fileName);
}

int main(int argc, char* argv[])
{
  // Test with these parameters: footbones.ply 2
  vtkNew<vtkNamedColors> colors;

  std::map<int, std::string> actorColors;
  actorColors[0] = "BurlyWood";
  actorColors[1] = "BurlyWood";
  actorColors[2] = "BurlyWood";

  std::map<int, std::string> rendererColors;
  rendererColors[0] = "Gainsboro";
  rendererColors[1] = "LightGrey";
  rendererColors[2] = "Silver";

  vtkSmartPointer<vtkPolyData> originalMesh;

  if (argc > 1) // If a file name is specified, open and use the file.
  {
    auto polyData = ReadPolyData(argc > 1 ? argv[1] : "");

    // Subdivision filters only work on triangles
    vtkNew<vtkTriangleFilter> triangles;
    triangles->SetInputData(polyData);
    triangles->Update();
    originalMesh = triangles->GetOutput();
  }
  else // If a file name is not specified, create a sphere
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();
    originalMesh = sphereSource->GetOutput();
  }

  std::cout << "Before subdivision" << std::endl;
  std::cout << "    There are " << originalMesh->GetNumberOfPoints()
            << " points." << std::endl;
  std::cout << "    There are " << originalMesh->GetNumberOfPolys()
            << " triangles." << std::endl;

  double numberOfViewports = 3.0;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(200 * numberOfViewports, 200); //(width, height)
  renderWindow->SetWindowName("Subdivision");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  renderWindowInteractor->SetRenderWindow(renderWindow);

  int numberOfSubdivisions = 2;
  for (unsigned i = 0; i < numberOfViewports; i++)
  {
    // Note: Here we create a superclass pointer (vtkPolyDataAlgorithm) so that
    // we can easily instantiate different types of subdivision filters.
    // Typically you would not want to do this, but rather create the pointer to
    // be the type filter you will actually use, e.g.
    // vtkNew<vtkLinearSubdivisionFilter>  subdivisionFilter;
    vtkSmartPointer<vtkPolyDataAlgorithm> subdivisionFilter;
    switch (i)
    {
    case 0:
      subdivisionFilter = vtkSmartPointer<vtkLinearSubdivisionFilter>::New();
      dynamic_cast<vtkLinearSubdivisionFilter*>(subdivisionFilter.GetPointer())
          ->SetNumberOfSubdivisions(numberOfSubdivisions);
      break;
    case 1:
      subdivisionFilter = vtkSmartPointer<vtkLoopSubdivisionFilter>::New();
      dynamic_cast<vtkLoopSubdivisionFilter*>(subdivisionFilter.GetPointer())
          ->SetNumberOfSubdivisions(numberOfSubdivisions);
      break;
    case 2:
      subdivisionFilter = vtkSmartPointer<vtkButterflySubdivisionFilter>::New();
      dynamic_cast<vtkButterflySubdivisionFilter*>(
          subdivisionFilter.GetPointer())
          ->SetNumberOfSubdivisions(numberOfSubdivisions);
      break;
    default:
      break;
    }
    subdivisionFilter->SetInputData(originalMesh);
    subdivisionFilter->Update();

    vtkNew<vtkRenderer> renderer;

    renderWindow->AddRenderer(renderer);
    renderer->SetViewport(static_cast<double>(i) / numberOfViewports, 0,
                          static_cast<double>(i + 1) / numberOfViewports, 1);

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(subdivisionFilter->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(
        colors->GetColor3d(actorColors[i]).GetData());

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d(rendererColors[i]).GetData());
    renderer->ResetCamera();

    renderWindow->Render();
  }

  renderWindowInteractor->Start();

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
    auto reader = vtkSmartPointer<vtkPLYReader>::New();
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtp")
  {
    auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".obj")
  {
    auto reader = vtkSmartPointer<vtkOBJReader>::New();
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".stl")
  {
    auto reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtk")
  {
    auto reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".g")
  {
    auto reader = vtkSmartPointer<vtkBYUReader>::New();
    reader->SetGeometryFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    // Return a polydata sphere if the extension is unknown.
    auto source = vtkSmartPointer<vtkSphereSource>::New();
    source->SetThetaResolution(20);
    source->SetPhiResolution(11);
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}
} // namespace
