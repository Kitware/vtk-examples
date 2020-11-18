#include <vtkActor.h>
#include <vtkAxes.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Readers
#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

#include <vtkPolyData.h>
#include <vtkSphereSource.h>

#include <algorithm> // For transform()
#include <cctype>    // For to_lower
#include <string>    // For find_last_of()

#include <array>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(std::string const& fileName);
void RotateX(vtkRenderWindow* renWin, vtkActor* actor);
void RotateY(vtkRenderWindow* renWin, vtkActor* actor);
void RotateZ(vtkRenderWindow* renWin, vtkActor* actor);
void RotateXY(vtkRenderWindow* renWin, vtkActor* actor);
} // namespace

int main(int argc, char* argv[])
{
  /*
  To match the illustrations in VTKTextbook.pdf, use BkgColor as the background
    and Wheat as the cow colour.
  Also comment out the lines:
    modelActor->GetProperty()->SetSpecular(.6);
    modelActor->GetProperty()->SetSpecularPower(30);
  and use cow.g as the input data.
  */

  auto figure = 0;
  std::string actorColor = "Wheat";

  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " filename [figure] [actorColor]"
              << std::endl;
    std::cout << "where: filename is the file cow.obj" << std::endl;
    std::cout << "       figure is 0, 1, 2, or 3, default 0" << std::endl;
    std::cout << "       actorColor: A color name, default Wheat." << std::endl;
    return EXIT_FAILURE;
  }

  std::string fileName = argv[1];

  if (argc == 3)
  {
    figure = atoi(argv[2]);
  }
  else if (argc > 3)
  {
    figure = atoi(argv[2]);
    actorColor = argv[3];
  }

  // Create renderer stuff
  //
  vtkNew<vtkNamedColors> colors;

  // Set the background color.
  std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
  // std::array<unsigned char, 4> bkg{{60, 93, 144, 255}};
  colors->SetColor("BkgColor", bkg.data());

  vtkNew<vtkRenderer> ren1;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetWindowName("Rotations");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Create the pipeline.
  //
  auto polyData = ReadPolyData(fileName.c_str());

  vtkNew<vtkPolyDataMapper> modelMapper;
  modelMapper->SetInputData(polyData);

  vtkNew<vtkActor> modelActor;
  modelActor->SetMapper(modelMapper);
  modelActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d(actorColor).GetData());

  if (actorColor != "Wheat")
  {
    modelActor->GetProperty()->SetSpecular(0.6);
    modelActor->GetProperty()->SetSpecularPower(30);
  }

  vtkNew<vtkAxes> modelAxesSource;
  modelAxesSource->SetScaleFactor(10);
  modelAxesSource->SetOrigin(0, 0, 0);

  vtkNew<vtkPolyDataMapper> modelAxesMapper;
  modelAxesMapper->SetInputConnection(modelAxesSource->GetOutputPort());

  vtkNew<vtkActor> modelAxes;
  modelAxes->SetMapper(modelAxesMapper);

  ren1->AddActor(modelAxes);
  modelAxes->VisibilityOff();

  // Add the actors to the renderer, set the background and size.
  //
  ren1->AddActor(modelActor);
  if (actorColor == "Wheat")
  {
    ren1->SetBackground(colors->GetColor3d("BkgColor").GetData());
  }
  else
  {
    ren1->SetBackground(colors->GetColor3d("Silver").GetData());
  }
  renWin->SetSize(640, 480);
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Azimuth(0);
  ren1->GetActiveCamera()->SetClippingRange(0.1, 1000.0);

  modelAxes->VisibilityOn();

  renWin->Render();
  renWin->Render();

  switch (figure)
  {
  default:
  case 0:
    RotateXY(renWin, modelActor);
    break;
  case 1:
    RotateX(renWin, modelActor);
    break;
  case 2:
    RotateY(renWin, modelActor);
    break;
  case 3:
    RotateZ(renWin, modelActor);
    break;
  }
  renWin->EraseOff();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {

void RotateX(vtkRenderWindow* renWin, vtkActor* actor)
{
  actor->SetOrientation(0, 0, 0);
  renWin->Render();
  renWin->Render();
  renWin->EraseOff();

  for (auto i = 0; i < 6; ++i)
  {
    actor->RotateX(60);
    renWin->Render();
    renWin->Render();
  }
  renWin->EraseOn();
}

void RotateY(vtkRenderWindow* renWin, vtkActor* actor)
{
  actor->SetOrientation(0, 0, 0);
  renWin->Render();
  renWin->EraseOff();

  for (auto i = 0; i < 6; ++i)
  {
    actor->RotateY(60);
    renWin->Render();
    renWin->Render();
  }
  renWin->EraseOn();
}

void RotateZ(vtkRenderWindow* renWin, vtkActor* actor)
{
  actor->SetOrientation(0, 0, 0);
  renWin->Render();
  renWin->EraseOff();

  for (auto i = 0; i < 6; ++i)
  {
    actor->RotateZ(60);
    renWin->Render();
    renWin->Render();
  }
  renWin->EraseOn();
}

void RotateXY(vtkRenderWindow* renWin, vtkActor* actor)
{
  actor->SetOrientation(0, 0, 0);
  actor->RotateX(60);
  renWin->Render();
  renWin->Render();
  renWin->EraseOff();

  for (auto i = 0; i < 6; ++i)
  {
    actor->RotateY(60);
    renWin->Render();
    renWin->Render();
  }
  renWin->EraseOn();
}

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
