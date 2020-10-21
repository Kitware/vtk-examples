#include <vtkBYUReader.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataPointSampler.h>
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

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  auto polyData = ReadPolyData(argc > 1 ? argv[1] : "");

  double bounds[6];
  polyData->GetBounds(bounds);
  double range[3];
  for (int i = 0; i < 3; ++i)
  {
    range[i] = bounds[2 * i + 1] - bounds[2 * i];
  }
  std::cout << "Range: " << range[0] << ", " << range[1] << ", " << range[2]
            << std::endl;
  std::cout << "# of original points: " << polyData->GetNumberOfPoints()
            << std::endl;
  vtkNew<vtkPolyDataPointSampler> sample;
  sample->SetInputData(polyData);
  sample->SetDistance(range[0] / 50);
  sample->Update();
  std::cout << "# of points after sampling: "
            << sample->GetOutput()->GetNumberOfPoints() << std::endl;

  ///
  double radius = range[0] * 0.01;
  vtkNew<vtkSphereSource> originalSource;
  originalSource->SetRadius(radius);

  vtkNew<vtkGlyph3D> originalGlyph3D;
  originalGlyph3D->SetInputData(polyData);
  originalGlyph3D->SetSourceConnection(originalSource->GetOutputPort());
  originalGlyph3D->ScalingOff();
  originalGlyph3D->Update();
  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputConnection(originalGlyph3D->GetOutputPort());
  originalMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetColor(
      colors->GetColor3d("Banana").GetData());

  ////
  vtkNew<vtkSphereSource> sampleSource;
  sampleSource->SetRadius(radius * 0.75);

  vtkNew<vtkGlyph3D> sampleGlyph3D;
  sampleGlyph3D->SetInputConnection(sample->GetOutputPort());
  sampleGlyph3D->SetSourceConnection(sampleSource->GetOutputPort());
  sampleGlyph3D->ScalingOff();
  sampleGlyph3D->Update();

  vtkNew<vtkPolyDataMapper> sampleMapper;
  sampleMapper->SetInputConnection(sampleGlyph3D->GetOutputPort());
  sampleMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> sampleActor;
  sampleActor->SetMapper(sampleMapper);
  sampleActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  // Create graphics stuff
  //
  vtkNew<vtkRenderer> ren1;
  ren1->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetSize(512, 512);
  renWin->SetWindowName("PolyDataPointSampler");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(originalActor);
  ren1->AddActor(sampleActor);

  // Generate an interesting view
  //
  ren1->GetActiveCamera()->SetPosition(1, 0, 0);
  ren1->GetActiveCamera()->SetFocalPoint(0, 1, 0);
  ren1->GetActiveCamera()->SetViewUp(0, 0, 1);
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Dolly(1.0);
  ren1->ResetCameraClippingRange();

  renWin->Render();
  iren->Initialize();
  iren->Start();

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
  else if (extension == ".vtk")
  {
    vtkNew<vtkPolyDataReader> reader;
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
  else if (extension == ".g")
  {
    vtkNew<vtkBYUReader> reader;
    reader->SetGeometryFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkSphereSource> sphere;
    sphere->SetRadius(1.0);
    sphere->SetPhiResolution(21);
    sphere->SetThetaResolution(41);
    sphere->Update();
    polyData = sphere->GetOutput();
  }
  return polyData;
}
} // namespace
