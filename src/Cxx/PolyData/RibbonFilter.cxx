#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRibbonFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Spiral parameters
  unsigned int nV = 256; // No. of vertices
  double rS = 2;         // Spiral radius
  unsigned int nCyc = 3; // No. of spiral cycles
  double h = 10;         // Height

  unsigned int i;

  // Create points and cells for a spiral
  vtkNew<vtkPoints> points;
  for (i = 0; i < nV; i++)
  {
    // Spiral coordinates
    auto vX = rS * cos(2 * vtkMath::Pi() * nCyc * i / (nV - 1));
    auto vY = rS * sin(2 * vtkMath::Pi() * nCyc * i / (nV - 1));
    auto vZ = h * i / nV;
    points->InsertPoint(i, vX, vY, vZ);
  }

  vtkNew<vtkCellArray> lines;
  lines->InsertNextCell(nV);
  for (i = 0; i < nV; i++)
  {
    lines->InsertCellPoint(i);
  }

  vtkNew<vtkPolyData> polyData;
  polyData->SetPoints(points);
  polyData->SetLines(lines);

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> lineMapper;
  lineMapper->SetInputData(polyData);

  vtkNew<vtkActor> lineActor;
  lineActor->SetMapper(lineMapper);
  lineActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
  lineActor->GetProperty()->SetLineWidth(3);

  // Create a ribbon around the line
  vtkNew<vtkRibbonFilter> ribbonFilter;
  ribbonFilter->SetInputData(polyData);
  ribbonFilter->SetWidth(.4);

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> ribbonMapper;
  ribbonMapper->SetInputConnection(ribbonFilter->GetOutputPort());

  vtkNew<vtkActor> ribbonActor;
  ribbonActor->SetMapper(ribbonMapper);
  ribbonActor->GetProperty()->SetColor(
      colors->GetColor3d("AliceBlue").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->SetWindowName("RibbonFilter");

  // Add the actor to the scene
  renderer->AddActor(ribbonActor);
  renderer->AddActor(lineActor);

  // Render and interact
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  // Generate an interesting view
  renderer->GetActiveCamera()->Azimuth(40);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCamera();
  renderer->ResetCameraClippingRange();

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
