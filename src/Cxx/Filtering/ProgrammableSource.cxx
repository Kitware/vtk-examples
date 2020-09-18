#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProgrammableSource.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVertexGlyphFilter.h>

// https://en.wikipedia.org/wiki/Lorenz_system
static void Lorenz(void* arg)
{
  double sigma = 10.0; /* The Lorenz paramaters */
  double beta = 8.0 / 3.0;
  double rho = 28.0;
  double h = .001; /* Integration step size */

  double x, y, z;
  double xx, yy, zz;
  x = 0.1;
  y = 0.1;
  z = 0.1;
  vtkNew<vtkPoints> points;
  // Get to a stable starting point
  for (int i = 0; i < 1000; ++i)
  {
    xx = x + h * sigma * (y - x);
    yy = y + h * (x * (rho - z) - y);
    zz = z + h * (x * y - (beta * z));
    x = xx;
    y = yy;
    z = zz;
  }
  for (int i = 0; i < 500000; ++i)
  {
    xx = x + h * sigma * (y - x);
    yy = y + h * (x * (rho - z) - y);
    zz = z + h * (x * y - (beta * z));
    points->InsertNextPoint(xx, yy, zz);
    x = xx;
    y = yy;
    z = zz;
  }
  vtkNew<vtkPolyData> pointsPolydata;
  pointsPolydata->SetPoints(points);
  vtkNew<vtkVertexGlyphFilter> vertexFilter;
  vertexFilter->SetInputData(pointsPolydata);
  vertexFilter->Update();
  vtkProgrammableSource* ps = static_cast<vtkProgrammableSource*>(arg);
  vtkPolyData* output = ps->GetPolyDataOutput();
  output->DeepCopy(vertexFilter->GetOutput());
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkProgrammableSource> source;
  source->SetExecuteMethod(Lorenz, source);
  source->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(source->GetPolyDataOutput());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("RosyBrown").GetData());

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  renderWindow->SetWindowName("ProgrammableSource");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
