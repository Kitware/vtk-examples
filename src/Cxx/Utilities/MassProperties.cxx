#include <vtkFillHolesFilter.h>
#include <vtkMassProperties.h>
#include <vtkNew.h>
#include <vtkPolyDataNormals.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>

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
  vtkSmartPointer<vtkPolyData> polyData = ReadPolyData(argc > 1 ? argv[1] : "");
  ;

  vtkNew<vtkFillHolesFilter> fillHolesFilter;
  fillHolesFilter->SetInputData(polyData);
  fillHolesFilter->SetHoleSize(1000.0);

  vtkNew<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInputConnection(fillHolesFilter->GetOutputPort());

  // Make the triangle windong order consistent
  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputConnection(triangleFilter->GetOutputPort());
  normals->ConsistencyOn();
  normals->SplittingOff();

  vtkNew<vtkMassProperties> massProperties;
  massProperties->SetInputConnection(normals->GetOutputPort());
  massProperties->Update();
  std::cout << "Volume: " << massProperties->GetVolume() << std::endl
            << "    VolumeX: " << massProperties->GetVolumeX() << std::endl
            << "    VolumeY: " << massProperties->GetVolumeY() << std::endl
            << "    VolumeZ: " << massProperties->GetVolumeZ() << std::endl
            << "Area:   " << massProperties->GetSurfaceArea() << std::endl
            << "    MinCellArea: " << massProperties->GetMinCellArea()
            << std::endl
            << "    MinCellArea: " << massProperties->GetMaxCellArea()
            << std::endl
            << "NormalizedShapeIndex: "
            << massProperties->GetNormalizedShapeIndex() << std::endl;

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
    source->SetPhiResolution(51);
    source->SetThetaResolution(51);
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}
} // namespace
