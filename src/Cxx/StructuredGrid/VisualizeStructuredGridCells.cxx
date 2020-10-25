#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShrinkFilter.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

#include <iostream>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a grid
  vtkNew<vtkStructuredGrid> structuredGrid;

  vtkNew<vtkPoints> points;
  unsigned int numi = 3;
  unsigned int numj = 4;
  unsigned int numk = 5;

  for (unsigned int k = 0; k < numk; k++)
  {
    for (unsigned int j = 0; j < numj; j++)
    {
      for (unsigned int i = 0; i < numi; i++)
      {
        points->InsertNextPoint(i, j, k);
      }
    }
  }

  // Specify the dimensions of the grid
  structuredGrid->SetDimensions(numi, numj, numk);
  structuredGrid->SetPoints(points);

  std::cout << "There are " << structuredGrid->GetNumberOfPoints()
            << " points before shrinking." << std::endl;
  std::cout << "There are " << structuredGrid->GetNumberOfCells()
            << " cells before shrinking." << std::endl;

  vtkNew<vtkShrinkFilter> shrinkFilter;
  shrinkFilter->SetInputData(structuredGrid);
  shrinkFilter->SetShrinkFactor(.8);
  shrinkFilter->Update();

  std::cout << "There are " << shrinkFilter->GetOutput()->GetNumberOfPoints()
            << " points after shrinking." << std::endl;
  std::cout << "There are " << shrinkFilter->GetOutput()->GetNumberOfCells()
            << " cells after shrinking." << std::endl;

  // Note: there are more points after shrinking because cells no longer share
  // points.

  // Create a mapper and actor
  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(shrinkFilter->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("VisualizeStructuredGridCells");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  // Render and interact
  renderWindow->Render();

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(9.56681, 4.14093, 5.85709);
  camera->SetFocalPoint(1, 1.5, 2);
  camera->SetViewUp(-0.172781, 0.94846, -0.26565);
  camera->SetDistance(9.7592);
  camera->SetClippingRange(5.84116, 14.7147);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
