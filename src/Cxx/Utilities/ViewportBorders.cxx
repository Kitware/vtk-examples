#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkCoordinate.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
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

namespace {
void ViewportBorder(vtkRenderer* renderer, double* color, bool last = false);
}

int main(int argc, char* argv[])
{
  //
  int numberOfFiles = argc - 1;
  if (numberOfFiles == 0)
  {
    std::cout << "Usage" << argv[0]
              << " file1 file2 file3 ... fileN e.g. v.vtk t.vtk k.vtk"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(300 * numberOfFiles, 300);
  renderWindow->SetWindowName("ViewportBorders");

  double size = 1.0 / numberOfFiles;
  for (unsigned int i = 0; static_cast<int>(i) < numberOfFiles; ++i)
  {
    vtkNew<vtkPolyDataReader> reader;
    reader->SetFileName(argv[i + 1]);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(reader->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Silver").GetData());

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    double viewport[4];
    viewport[0] = size * i;
    viewport[1] = 0.0;
    viewport[2] = size * (i + 1);
    viewport[3] = 1.0;
    renderer->SetViewport(viewport);
    ViewportBorder(renderer, colors->GetColor3d("Gold").GetData(),
                   static_cast<int>(i) == numberOfFiles - 1);
    renderWindow->AddRenderer(renderer);
  }

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
namespace {
// draw the borders of a renderer's viewport
void ViewportBorder(vtkRenderer* renderer, double* color, bool last)
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

  // now make tge polydata and display it
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
