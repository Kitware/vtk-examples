#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRectilinearGrid.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShrinkFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a grid
  vtkNew<vtkRectilinearGrid> grid;

  grid->SetDimensions(2, 3, 2);

  vtkNew<vtkDoubleArray> xArray;
  xArray->InsertNextValue(0.0);
  xArray->InsertNextValue(2.0);

  vtkNew<vtkDoubleArray> yArray;
  yArray->InsertNextValue(0.0);
  yArray->InsertNextValue(1.0);
  yArray->InsertNextValue(2.0);

  vtkNew<vtkDoubleArray> zArray;
  zArray->InsertNextValue(0.0);
  zArray->InsertNextValue(5.0);

  grid->SetXCoordinates(xArray);
  grid->SetYCoordinates(yArray);
  grid->SetZCoordinates(zArray);

  vtkNew<vtkShrinkFilter> shrinkFilter;
  shrinkFilter->SetInputData(grid);
  shrinkFilter->SetShrinkFactor(.8);

  // Create a mapper and actor
  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(shrinkFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("PeachPuff").GetData());

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("VisualizeRectilinearGrid");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->GetActiveCamera()->Roll(10.0);
  renderer->GetActiveCamera()->Elevation(60.0);
  renderer->GetActiveCamera()->Azimuth(30.0);
  renderer->ResetCamera();

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
