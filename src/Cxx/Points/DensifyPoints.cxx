#include <vtkCamera.h>
#include <vtkDensifyPointCloudFilter.h>
#include <vtkGlyph3DMapper.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <vtksys/SystemTools.hxx>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

int main(int argc, char* argv[])
{
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

  double maxRange = std::max(std::max(range[0], range[1]), range[2]);

  std::cout << "# of original points: " << polyData->GetNumberOfPoints()
            << std::endl;
  vtkNew<vtkDensifyPointCloudFilter> densify;
  densify->SetInputData(polyData);
  densify->SetMaximumNumberOfIterations(5);
  densify->SetTargetDistance(maxRange * .03);
  densify->SetNumberOfClosestPoints(10);
  densify->Update();
  std::cout << "# of densified points: "
            << densify->GetOutput()->GetNumberOfPoints() << std::endl;

  vtkNew<vtkNamedColors> colors;
  ///
  double radius = maxRange * .01;
  vtkNew<vtkSphereSource> sphereSource1;
  sphereSource1->SetRadius(radius);

  vtkNew<vtkGlyph3DMapper> glyph3D1;
  glyph3D1->SetInputData(polyData);
  glyph3D1->SetSourceConnection(sphereSource1->GetOutputPort());
  glyph3D1->ScalarVisibilityOff();
  glyph3D1->ScalingOff();

  vtkNew<vtkActor> glyph3DActor1;
  glyph3DActor1->SetMapper(glyph3D1);
  glyph3DActor1->GetProperty()->SetColor(
      colors->GetColor3d("Banana").GetData());

  ////
  vtkNew<vtkSphereSource> sphereSource2;
  sphereSource2->SetRadius(radius * .75);

  vtkNew<vtkGlyph3DMapper> glyph3D2;
  glyph3D2->SetInputConnection(densify->GetOutputPort());
  glyph3D2->SetSourceConnection(sphereSource2->GetOutputPort());
  glyph3D2->ScalarVisibilityOff();
  glyph3D2->ScalingOff();

  vtkNew<vtkActor> glyph3DActor2;
  glyph3DActor2->SetMapper(glyph3D2);
  glyph3DActor2->GetProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());

  // Create graphics stuff
  //
  vtkNew<vtkRenderer> ren1;
  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetSize(512, 512);
  renWin->SetWindowName("DensifyPoints");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(glyph3DActor1);
  ren1->AddActor(glyph3DActor2);

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
    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->SetSeed(8775070);

    vtkNew<vtkPointSource> points;
    points->SetNumberOfPoints(100000);
    points->SetRadius(10.0);
    double x, y, z;
    // random position
    x = randomSequence->GetRangeValue(-100, 100);
    randomSequence->Next();
    y = randomSequence->GetRangeValue(-100, 100);
    randomSequence->Next();
    z = randomSequence->GetRangeValue(-100, 100);
    randomSequence->Next();
    points->SetCenter(x, y, z);
    points->SetDistributionToShell();
    points->Update();
    polyData = points->GetOutput();
  }
  return polyData;
}
} // namespace
