#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCleanPolyData.h>
#include <vtkColor.h>
#include <vtkImplicitBoolean.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
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
  vtkSmartPointer<vtkPolyData> polyData = ReadPolyData(argc > 1 ? argv[1] : "");

  vtkNew<vtkNamedColors> colors;

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("White").GetData());

  // Create background colors for each viewport
  std::vector<vtkColor3d> backgroundColors;
  backgroundColors.push_back(colors->GetColor3d("Cornsilk"));
  backgroundColors.push_back(colors->GetColor3d("NavajoWhite"));
  backgroundColors.push_back(colors->GetColor3d("Tan"));

  // Create a renderer for each view port
  std::vector<vtkSmartPointer<vtkRenderer>> ren;
  ren.push_back(vtkSmartPointer<vtkRenderer>::New());
  ren.push_back(vtkSmartPointer<vtkRenderer>::New());
  ren.push_back(vtkSmartPointer<vtkRenderer>::New());
  ren[0]->SetViewport(0, 0, 1.0 / 3.0, 1);         // Input
  ren[1]->SetViewport(1.0 / 3.0, 0, 2.0 / 3.0, 1); // Normals (no split)
  ren[2]->SetViewport(2.0 / 3.0, 0, 1, 1);         // Normals (split)

  // Shared camera
  vtkNew<vtkCamera> camera;

  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputData(polyData);
  normals->SetFeatureAngle(30.0);
  for (int i = 0; i < 3; ++i)
  {
    if (i == 0)
    {
      normals->ComputePointNormalsOff();
    }
    else if (i == 1)
    {
      normals->ComputePointNormalsOn();
      normals->SplittingOff();
    }
    else
    {
      normals->ComputePointNormalsOn();
      normals->SplittingOn();
    }

    normals->Update();

    vtkNew<vtkPolyData> normalsPolyData;
    normalsPolyData->DeepCopy(normals->GetOutput());

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(normalsPolyData);
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetDiffuseColor(
        colors->GetColor3d("Peacock").GetData());
    actor->GetProperty()->SetDiffuse(.7);
    actor->GetProperty()->SetSpecularPower(20);
    actor->GetProperty()->SetSpecular(.5);

    // add the actor
    ren[i]->SetBackground(backgroundColors[i].GetData());
    ren[i]->SetActiveCamera(camera);
    ren[i]->AddActor(actor);
  }

  // render window
  vtkNew<vtkRenderWindow> renwin;
  renwin->AddRenderer(ren[0]);
  renwin->AddRenderer(ren[1]);
  renwin->AddRenderer(ren[2]);
  renwin->SetWindowName("NormalsDemo");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renwin);

  renwin->SetSize(900, 300);
  ren[0]->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  ren[0]->GetActiveCamera()->SetPosition(1, 0, 0);
  ren[0]->GetActiveCamera()->SetViewUp(0, 0, -1);
  ren[0]->ResetCamera();

  ren[0]->GetActiveCamera()->Azimuth(120);
  ren[0]->GetActiveCamera()->Elevation(30);
  ren[0]->GetActiveCamera()->Dolly(1.1);
  ren[0]->ResetCameraClippingRange();

  renwin->Render();
  ren[0]->ResetCamera();
  renwin->Render();

  // Start
  interactor->Initialize();
  interactor->Start();

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
    sphere->SetPhiResolution(11);
    sphere->SetThetaResolution(11);

    sphere->Update();
    polyData = sphere->GetOutput();
  }
  vtkNew<vtkCleanPolyData> clean;
  clean->SetInputData(polyData);
  clean->Update();
  return clean->GetOutput();
}
} // namespace
