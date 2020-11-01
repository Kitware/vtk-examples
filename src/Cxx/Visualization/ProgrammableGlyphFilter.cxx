#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProgrammableGlyphFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

void CalcGlyph(void* arg)
{

  vtkProgrammableGlyphFilter* glyphFilter = (vtkProgrammableGlyphFilter*)arg;

  if (!glyphFilter)
  {
    std::cerr << "glyphFilter is not valid!" << std::endl;
  }
  double pointCoords[3];
  glyphFilter->GetPoint(pointCoords);

  std::cout << "Calling CalcGlyph for point " << glyphFilter->GetPointId()
            << std::endl;
  std::cout << "Point coords are: " << pointCoords[0] << " " << pointCoords[1]
            << " " << pointCoords[2] << std::endl;
  if (glyphFilter->GetPointId() == 0)
  {
    vtkNew<vtkConeSource> coneSource;
    coneSource->SetCenter(pointCoords);
    glyphFilter->SetSourceConnection(coneSource->GetOutputPort());
  }
  else if (glyphFilter->GetPointId() == 1)
  {
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->SetCenter(pointCoords);
    glyphFilter->SetSourceConnection(cubeSource->GetOutputPort());
  }
  else if (glyphFilter->GetPointId() == 2)
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(pointCoords);
    glyphFilter->SetSourceConnection(sphereSource->GetOutputPort());
  }
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create points
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(5, 0, 0);
  points->InsertNextPoint(10, 0, 0);

  // Combine into a polydata
  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);

  vtkNew<vtkProgrammableGlyphFilter> glyphFilter;
  glyphFilter->SetInputData(polydata);
  glyphFilter->SetGlyphMethod(CalcGlyph, glyphFilter);
  // need a default glyph, but this should not be used
  vtkNew<vtkConeSource> coneSource;
  glyphFilter->SetSourceConnection(coneSource->GetOutputPort());

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(glyphFilter->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ProgrammableGlyphFilter");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->Render();

  renderer->GetActiveCamera()->Zoom(0.9);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
