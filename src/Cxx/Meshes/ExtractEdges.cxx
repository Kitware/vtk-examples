#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkExtractEdges.h>
#include <vtkLine.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

// Note that:
// vtkExtractEdges moved from vtkFiltersExtraction to vtkFiltersCore in
// VTK commit d9981b9aeb93b42d1371c6e295d76bfdc18430bd

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  std::cout << "Sphere" << endl << "----------" << endl;
  std::cout << "There are " << sphereSource->GetOutput()->GetNumberOfCells()
            << " cells." << std::endl;
  std::cout << "There are " << sphereSource->GetOutput()->GetNumberOfPoints()
            << " points." << std::endl;

  vtkNew<vtkExtractEdges> extractEdges;
  extractEdges->SetInputConnection(sphereSource->GetOutputPort());
  extractEdges->Update();

  vtkCellArray* lines = extractEdges->GetOutput()->GetLines();
  vtkPoints* points = extractEdges->GetOutput()->GetPoints();

  std::cout << std::endl << "Edges" << endl << "----------" << std::endl;
  std::cout << "There are " << lines->GetNumberOfCells() << " cells."
            << std::endl;
  std::cout << "There are " << points->GetNumberOfPoints() << " points."
            << std::endl;

  // Traverse all of the edges
  for (vtkIdType i = 0; i < extractEdges->GetOutput()->GetNumberOfCells(); i++)
  {
    // std::cout << "Type: " <<
    // extractEdges->GetOutput()->GetCell(i)->GetClassName() << std::endl;
    auto line = dynamic_cast<vtkLine*>(extractEdges->GetOutput()->GetCell(i));
    std::cout << "Line " << i << " : " << *line << std::endl;
  }

  // Visualize the edges

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(extractEdges->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ExtractEdges");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(
      colors->GetColor3d("RoyalBlue").GetData()); // Background color white

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
