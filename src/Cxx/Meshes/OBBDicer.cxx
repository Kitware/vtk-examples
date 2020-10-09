#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBBDicer.h>
#include <vtkOutlineCornerFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>
#include <vtksys/SystemTools.hxx>

#include <random>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

int main(int argc, char* argv[])
{
  int pieces = 4;
  if (argc > 2)
  {
    pieces = std::atoi(argv[2]);
  }

  auto inputPolyData = ReadPolyData(argc > 1 ? argv[1] : "");
  ;

  // Create pipeline
  vtkNew<vtkOBBDicer> dicer;
  dicer->SetInputData(inputPolyData);
  dicer->SetNumberOfPieces(pieces);
  dicer->SetDiceModeToSpecifiedNumberOfPieces();
  dicer->Update();

  int numberOfRegions = dicer->GetNumberOfActualPieces();

  // Fill in a few known colors, the rest will be generated if needed
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkLookupTable> lut;
  lut->SetNumberOfTableValues(std::max(numberOfRegions, 10));
  lut->Build();
  lut->SetTableValue(0, colors->GetColor4d("Gold").GetData());
  lut->SetTableValue(1, colors->GetColor4d("Banana").GetData());
  lut->SetTableValue(2, colors->GetColor4d("Tomato").GetData());
  lut->SetTableValue(3, colors->GetColor4d("Wheat").GetData());
  lut->SetTableValue(4, colors->GetColor4d("Lavender").GetData());
  lut->SetTableValue(5, colors->GetColor4d("Flesh").GetData());
  lut->SetTableValue(6, colors->GetColor4d("Raspberry").GetData());
  lut->SetTableValue(7, colors->GetColor4d("Salmon").GetData());
  lut->SetTableValue(8, colors->GetColor4d("Mint").GetData());
  lut->SetTableValue(9, colors->GetColor4d("Peacock").GetData());

  // If the number of regions is larger than the number of specified colors,
  // generate some random colors.
  // Note: If a Python version is written, it is probably best to use
  //       vtkMinimalStandardRandomSequence in it and here, to ensure
  //       that the random number generation is the same.
  if (numberOfRegions > 9)
  {
    std::mt19937 mt(4355412); // Standard mersenne_twister_engine
    std::uniform_real_distribution<double> distribution(.6, 1.0);
    for (auto i = 10; i < numberOfRegions; ++i)
    {
      lut->SetTableValue(i, distribution(mt), distribution(mt),
                         distribution(mt), 1.0);
    }
  }

  vtkNew<vtkPolyDataMapper> inputMapper;
  inputMapper->SetInputConnection(dicer->GetOutputPort());
  inputMapper->SetScalarRange(0, dicer->GetNumberOfActualPieces());
  inputMapper->SetLookupTable(lut);

  std::cout << "Asked for: " << dicer->GetNumberOfPieces()
            << " pieces, got: " << dicer->GetNumberOfActualPieces()
            << std::endl;

  vtkNew<vtkActor> inputActor;
  inputActor->SetMapper(inputMapper);
  inputActor->GetProperty()->SetInterpolationToFlat();

  vtkNew<vtkOutlineCornerFilter> outline;
  outline->SetInputData(inputPolyData);

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  vtkNew<vtkRenderer> renderer;
  renderer->UseHiddenLineRemovalOn();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("OBBDicer");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer, set the background and size
  renderer->AddActor(outlineActor);
  renderer->AddActor(inputActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->GetActiveCamera()->Azimuth(150);
  renderer->GetActiveCamera()->Elevation(15);
  renderer->ResetCamera();

  // Render the image
  renderWindow->Render();

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
    source->SetPhiResolution(25);
    source->SetThetaResolution(25);
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}
} // namespace
