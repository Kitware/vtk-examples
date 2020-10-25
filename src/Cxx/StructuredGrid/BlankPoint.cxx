#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkIdList.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLStructuredGridWriter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a grid
  vtkNew<vtkStructuredGrid> structuredGrid;

  vtkNew<vtkPoints> points;

  unsigned int gridSize = 8;
  unsigned int counter = 0;
  // Create a 5x5 grid of points
  for (unsigned int j = 0; j < gridSize; j++)
  {
    for (unsigned int i = 0; i < gridSize; i++)
    {
      if (i == 3 && j == 3) // Make one point higher than the rest
      {
        points->InsertNextPoint(i, j, 2);
        std::cout << "The different point is number " << counter << std::endl;
      }
      else
      {
        // Make most of the points the same height
        points->InsertNextPoint(i, j, 0);
      }
      counter++;
    }
  }

  // Specify the dimensions of the grid
  structuredGrid->SetDimensions(gridSize, gridSize, 1);

  structuredGrid->SetPoints(points);

  structuredGrid->BlankPoint(27);
  structuredGrid->Modified();

  // Create a mapper and actor
  vtkNew<vtkDataSetMapper> gridMapper;
  gridMapper->SetInputData(structuredGrid);

  vtkNew<vtkActor> gridActor;
  gridActor->SetMapper(gridMapper);
  gridActor->GetProperty()->EdgeVisibilityOn();
  gridActor->GetProperty()->SetEdgeColor(colors->GetColor3d("Blue").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("BlankPoint");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(gridActor);
  renderer->SetBackground(colors->GetColor3d("ForestGreen").GetData());

  // Render and interact
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
