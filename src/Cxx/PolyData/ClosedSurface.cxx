#include <vtkFeatureEdges.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

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
  std::string fileName = "vtkSphereSource";
  if (argc > 1)
  {
    fileName = argv[1];
  }

  vtkSmartPointer<vtkPolyData> polyData = ReadPolyData(fileName.c_str());

  vtkNew<vtkFeatureEdges> featureEdges;
  featureEdges->FeatureEdgesOff();
  featureEdges->BoundaryEdgesOn();
  featureEdges->NonManifoldEdgesOn();
  featureEdges->ManifoldEdgesOff();
  featureEdges->SetInputData(polyData);
  featureEdges->Update();

  int numberOfOpenEdges = featureEdges->GetOutput()->GetNumberOfCells();

  if (numberOfOpenEdges > 0)
  {
    std::cout << fileName << ": Surface is not closed" << std::endl;
  }
  else
  {
    std::cout << fileName << ": Surface is closed" << std::endl;
  }

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
