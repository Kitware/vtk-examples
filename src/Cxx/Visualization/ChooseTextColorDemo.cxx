#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCoordinate.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

// For drawing the vieport border.
#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkCoordinate.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPolyLine.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>

namespace {
// Given a color, find a contrasting color. If the given color is "light",
// use the lightColor otherwise use the darkColor
void ChooseContrastingColor(double* rgbIn, double* rgbOut,
                            double threshold = .5,
                            const std::string& lightColor = "white",
                            const std::string& darkColor = "black");

void ViewportBorder(vtkRenderer* renderer, double* color, bool last = false);

void RandomHSV(double hsv[3], double const& min_r, double const& max_r,
               vtkMinimalStandardRandomSequence* rng);

} // namespace

int main(int argc, char* argv[])
{
  // For testing
  vtkNew<vtkMinimalStandardRandomSequence> rng;
  rng->SetSeed(8775070);
  // rng->SetSeed(0);
  // rng->SetSeed(4355412);

  double threshold = .5;
  std::string lightColor = "white";
  std::string darkColor = "black";
  if (argc > 1)
  {
    threshold = atof(argv[1]);
  }
  if (argc > 2)
  {
    lightColor = argv[2];
  }
  if (argc > 3)
  {
    darkColor = argv[3];
  }
  // Visualize
  vtkNew<vtkNamedColors> colors;

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  std::vector<vtkSmartPointer<vtkRenderer>> renderers;
  unsigned int xGridDimensions = 10;
  unsigned int yGridDimensions = 10;
  auto min_r = 0.0;
  auto max_r = 1.0;
  for (auto i = 0; i < static_cast<int>(xGridDimensions * yGridDimensions); ++i)
  {
    // Create textActors
    vtkNew<vtkTextActor> textActor;
    textActor->GetTextProperty()->SetJustificationToCentered();
    textActor->GetTextProperty()->SetVerticalJustificationToCentered();
    textActor->SetInput("Text");
    textActor->SetPosition(50, 50);
    textActor->GetTextProperty()->BoldOff();
    textActor->GetTextProperty()->SetFontSize(20);

    // Setup renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(textActor);
    double hsv[3];
    RandomHSV(hsv, min_r, max_r, rng);
    double rgb[3];
    vtkMath::HSVToRGB(hsv, rgb);
    renderer->SetBackground(rgb);
    // Compute a good color for text on the renderer background
    ChooseContrastingColor(renderer->GetBackground(), rgb, threshold,
                           lightColor, darkColor);
    textActor->GetTextProperty()->SetColor(rgb);
    renderers.push_back(renderer);
    renderWindow->AddRenderer(renderer);
  }

  // Setup viewports for the renderers
  int rendererSize = 100;
  renderWindow->SetSize(rendererSize * xGridDimensions,
                        rendererSize * yGridDimensions);
  renderWindow->SetWindowName("ChooseTextColorDemo");

  for (auto row = 0; row < static_cast<int>(yGridDimensions); row++)
  {
    for (auto col = 0; col < static_cast<int>(xGridDimensions); col++)
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
      ViewportBorder(renderers[index], colors->GetColor3d("White").GetData(),
                     col == static_cast<int>(xGridDimensions));
    }
  }
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renderWindowInteractor->Start();

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

void ChooseContrastingColor(double* rgbIn, double* rgbOut, double threshold,
                            const std::string& lightColor,
                            const std::string& darkColor)
{
  vtkNew<vtkNamedColors> colors;

  double hsv[3];
  // If the value is <= .5, use a light color, otherwise use a dark color
  vtkMath::RGBToHSV(rgbIn, hsv);
  if (hsv[2] <= threshold)
  {
    colors->GetColor(lightColor.c_str(), rgbOut[0], rgbOut[1], rgbOut[2]);
  }
  else
  {
    colors->GetColor(darkColor.c_str(), rgbOut[0], rgbOut[1], rgbOut[2]);
  }
}

void RandomHSV(double hsv[3], double const& min_r, double const& max_r,
               vtkMinimalStandardRandomSequence* rng)
{
  for (auto i = 0; i < 3; ++i)
  {
    hsv[i] = rng->GetRangeValue(min_r, max_r);
    rng->Next();
  }
}

} // namespace
