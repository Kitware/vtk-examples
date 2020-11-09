#include <vtkActor.h>
#include <vtkBYUReader.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkShepardMethod.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>
#include <vtksys/SystemTools.hxx>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

int main(int argc, char* argv[])
{
  // Parse input arguments
  if (argc < 3)
  {
    std::cout
        << "Required parameters: Filename  resolution e.g. cowHead.vtp 100"
        << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  auto polyData = ReadPolyData(argc > 1 ? argv[1] : "");

  unsigned int resolution = 100;
  if (argc >= 3)
  {
    resolution = std::atoi(argv[2]);
  }
  vtkNew<vtkShepardMethod> interpolator;
  interpolator->SetInputData(polyData);
  interpolator->SetModelBounds(polyData->GetBounds());
  interpolator->SetSampleDimensions(resolution, resolution, resolution);
  interpolator->SetNullValue(-10000);
  interpolator->Update();
  std::cout << "Scalar Range: "
            << interpolator->GetOutput()->GetScalarRange()[0] << ", "
            << interpolator->GetOutput()->GetScalarRange()[1] << std::endl;

  vtkNew<vtkProbeFilter> probe;
  probe->SetInputData(0, polyData);
  probe->SetInputConnection(1, interpolator->GetOutputPort());

  vtkNew<vtkContourFilter> interpolatedContour;
  interpolatedContour->SetInputConnection(probe->GetOutputPort());
  interpolatedContour->SetValue(0, 0.0);

  vtkNew<vtkContourFilter> originalContour;
  originalContour->SetInputData(polyData);
  originalContour->SetValue(0, 0.0);

  vtkNew<vtkPolyDataMapper> interpolatedMapper;
  interpolatedMapper->SetInputConnection(interpolatedContour->GetOutputPort());
  interpolatedMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> interpolatedActor;
  interpolatedActor->SetMapper(interpolatedMapper);
  interpolatedActor->GetProperty()->SetColor(
      colors->GetColor3d("Banana").GetData());
  interpolatedActor->GetProperty()->SetLineWidth(4.0);

  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputConnection(originalContour->GetOutputPort());
  originalMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());
  originalActor->GetProperty()->SetLineWidth(4.0);
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ShepardInterpolation");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(interpolatedActor);
  renderer->AddActor(originalActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(120);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.5);
  renderer->ResetCameraClippingRange();

  renderWindow->SetSize(640, 480);
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
