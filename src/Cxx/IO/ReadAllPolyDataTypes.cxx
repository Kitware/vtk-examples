#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTimerLog.h>
#include <vtksys/SystemTools.hxx>

#include <algorithm>
#include <array>
#include <random>
#include <string>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

int main(int argc, char* argv[])
{
  // Vis Pipeline
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 480);
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->SetBackground(colors->GetColor3d("Wheat").GetData());
  renderer->UseHiddenLineRemovalOn();

  // Note: If a Python version is written, it is probably best to use
  //       vtkMinimalStandardRandomSequence in it and here, to ensure
  //       that the random number generation is the same.
  std::mt19937 mt(4355412); // Standard mersenne_twister_engine
  std::uniform_real_distribution<double> distribution(0.6, 1.0);

  // PolyData file pipeline
  for (int i = 1; i < argc; ++i)
  {
    std::cout << "Loading: " << argv[i] << std::endl;
    auto polyData = ReadPolyData(argv[i]);

    // Visualize
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    std::array<double, 3> randomColor;
    randomColor[0] = distribution(mt);
    randomColor[1] = distribution(mt);
    randomColor[2] = distribution(mt);
    vtkNew<vtkProperty> backProp;
    backProp->SetDiffuseColor(colors->GetColor3d("Banana").GetData());
    backProp->SetSpecular(0.6);
    backProp->SetSpecularPower(30);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetBackfaceProperty(backProp);
    actor->GetProperty()->SetDiffuseColor(randomColor.data());
    actor->GetProperty()->SetSpecular(0.3);
    actor->GetProperty()->SetSpecularPower(30);
    renderer->AddActor(actor);
  }

  renderWindow->SetWindowName("ReadAllPolyDataTypes");
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension =
      vtksys::SystemTools::GetFilenameLastExtension(std::string(fileName));

  // Drop the case of the extension
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 ::tolower);

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
