#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkBYUReader.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCoordinate.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkTimerLog.h>
#include <vtkXMLPolyDataReader.h>

#include <vtksys/SystemTools.hxx>

#include <algorithm>
#include <string>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);

void ViewportBorder(vtkSmartPointer<vtkRenderer>& renderer, double* color,
                    bool last = false);
} // namespace

int main(int argc, char* argv[])
{
  // Visualize
  vtkNew<vtkNamedColors> colors;

  // Create one text property for all
  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetFontSize(16);
  textProperty->SetColor(0.3, 0.3, 0.3);

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  std::vector<vtkSmartPointer<vtkRenderer>> renderers;
  for (auto i = 1; i < argc; ++i)
  {
    std::cout << argv[i] << std::endl;
    auto polyData = ReadPolyData(argv[i]);
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetDiffuseColor(
        colors->GetColor3d("Light_salmon").GetData());
    actor->GetProperty()->SetSpecular(0.6);
    actor->GetProperty()->SetSpecularPower(30);

    // Create textActors
    vtkNew<vtkTextMapper> textMapper;
    textMapper->SetTextProperty(textProperty);
    textMapper->SetInput(vtksys::SystemTools::GetFilenameName(argv[i]).c_str());

    vtkNew<vtkActor2D> textActor;
    textActor->SetMapper(textMapper);
    textActor->SetPosition(20, 20);

    // Setup renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(textActor);
    renderer->SetBackground(colors->GetColor3d("mint").GetData());
    renderers.push_back(renderer);
    renderWindow->AddRenderer(renderer);
  }

  // Setup viewports for the renderers
  auto rendererSize = 400;
  auto xGridDimensions = 3;
  auto yGridDimensions = 2;
  yGridDimensions = argc / xGridDimensions;
  renderWindow->SetSize(rendererSize * xGridDimensions,
                        rendererSize * yGridDimensions);
  auto blank = argc - 1 + ((argc - 1) % xGridDimensions);
  for (auto i = argc; i < blank; ++i)
  {
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("White").GetData());
    renderers.push_back(renderer);
    renderWindow->AddRenderer(renderer);
  }

  for (auto row = 0; row < yGridDimensions; row++)
  {
    for (auto col = 0; col < xGridDimensions; col++)
    {
      auto index = row * xGridDimensions + col;

      // (xmin, ymin, xmax, ymax)
      double viewport[4] = {static_cast<double>(col) * rendererSize /
                                (xGridDimensions * rendererSize),
                            static_cast<double>(yGridDimensions - (row + 1)) *
                                rendererSize / (yGridDimensions * rendererSize),
                            static_cast<double>(col + 1) * rendererSize /
                                (xGridDimensions * rendererSize),
                            static_cast<double>(yGridDimensions - row) *
                                rendererSize /
                                (yGridDimensions * rendererSize)};
      renderers[index]->SetViewport(viewport);
      ViewportBorder(renderers[index],
                     colors->GetColor3d("SlateGray").GetData(),
                     col == static_cast<int>(xGridDimensions));
    }
  }
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderWindow->SetWindowName("ReadAllPolyDataTypesDemo");
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

// draw the borders of a renderer's viewport
void ViewportBorder(vtkSmartPointer<vtkRenderer>& renderer, double* color,
                    bool last)
{
  // points start at upper right and proceed anti-clockwise
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(4);
  points->InsertPoint(0, 1, 1, 0);
  points->InsertPoint(1, 0, 1, 0);
  points->InsertPoint(2, 0, 0, 0);
  points->InsertPoint(3, 1, 0, 0);

  // create cells, and lines
  vtkNew<vtkCellArray> cells;
  cells->Initialize();

  vtkNew<vtkPolyLine> lines;

  // only draw last line if this is the last viewport
  // this prevents double vertical lines at right border
  // if different colors are used for each border, then do
  // not specify last
  if (last)
  {
    lines->GetPointIds()->SetNumberOfIds(5);
  }
  else
  {
    lines->GetPointIds()->SetNumberOfIds(4);
  }
  for (unsigned int i = 0; i < 4; ++i)
  {
    lines->GetPointIds()->SetId(i, i);
  }
  if (last)
  {
    lines->GetPointIds()->SetId(4, 0);
  }
  cells->InsertNextCell(lines);

  // now make the polydata and display it
  vtkNew<vtkPolyData> poly;
  poly->Initialize();
  poly->SetPoints(points);
  poly->SetLines(cells);

  // use normalized viewport coordinates since
  // they are independent of window size
  vtkNew<vtkCoordinate> coordinate;
  coordinate->SetCoordinateSystemToNormalizedViewport();

  vtkNew<vtkPolyDataMapper2D> mapper;
  mapper->SetInputData(poly);
  mapper->SetTransformCoordinate(coordinate);

  vtkNew<vtkActor2D> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(color);

  // line width should be at least 2 to be visible at extremes
  actor->GetProperty()->SetLineWidth(4.0); // Line Width

  renderer->AddViewProp(actor);
}

} // namespace
