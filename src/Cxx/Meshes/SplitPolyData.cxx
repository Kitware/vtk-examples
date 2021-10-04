#include <vtkActor.h>
#include <vtkColorSeries.h>
#include <vtkDataSetMapper.h>
#include <vtkGeometryFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBBDicer.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkThreshold.h>
#include <vtkXMLPolyDataWriter.h>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

#include <vtksys/SystemTools.hxx>

#include <sstream>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> namedColors;

  std::string filename;
  std::string extension;

  vtkSmartPointer<vtkPolyData> inputPolyData =
      ReadPolyData(argc > 1 ? argv[1] : "");
  ;
  if (argc > 1)
  {
    filename =
        vtksys::SystemTools::GetFilenameWithoutExtension(std::string(argv[1]));
    extension = vtksys::SystemTools::GetFilenameExtension(std::string(argv[1]));
  }
  else
  {
    filename = "sphere";
    extension = ".vtp";
  }

  // Create pipeline
  vtkNew<vtkOBBDicer> dicer;
  dicer->SetInputData(inputPolyData);
  dicer->SetNumberOfPieces(4);
  dicer->SetDiceModeToSpecifiedNumberOfPieces();
  dicer->Update();

  vtkNew<vtkThreshold> selector;
  selector->SetInputArrayToProcess(0, 0, 0, 0, "vtkOBBDicer_GroupIds");
  selector->SetInputConnection(dicer->GetOutputPort());
  selector->AllScalarsOff();

  // Create graphics stuff
  //
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(namedColors->GetColor3d("NavajoWhite").GetData());

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  renWin->SetSize(512, 512);
  renWin->SetWindowName("SplitPolyData");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Use a color series to create a transfer function
  vtkNew<vtkColorSeries> colorSeries;
  colorSeries->SetColorScheme(vtkColorSeries::BREWER_DIVERGING_SPECTRAL_11);

  // Create an actor for each piece
  for (int i = 0; i < dicer->GetNumberOfActualPieces(); ++i)
  {
    selector->SetLowerThreshold(i);
    selector->SetUpperThreshold(i);
    vtkNew<vtkGeometryFilter> geometry;
    geometry->SetInputConnection(selector->GetOutputPort());
    geometry->Update();

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(geometry->GetOutput());
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkColor3ub color = colorSeries->GetColor(i);
    double dColor[3];
    dColor[0] = static_cast<double>(color[0]) / 255.0;
    dColor[1] = static_cast<double>(color[1]) / 255.0;
    dColor[2] = static_cast<double>(color[2]) / 255.0;
    actor->GetProperty()->SetColor(dColor);
    renderer->AddActor(actor);
  }
  renWin->Render();
  iren->Initialize();
  iren->Start();

  vtkNew<vtkGeometryFilter> geometry;
  geometry->SetInputConnection(selector->GetOutputPort());

  vtkNew<vtkXMLPolyDataWriter> writer;
  writer->SetInputConnection(geometry->GetOutputPort());
  for (int i = 0; i < dicer->GetNumberOfActualPieces(); ++i)
  {
    std::stringstream pieceName;
    pieceName << filename << "_" << i + 1 << extension;
    selector->SetLowerThreshold(i);
    selector->SetUpperThreshold(i);
    writer->SetFileName(pieceName.str().c_str());
    writer->Write();
  }

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
