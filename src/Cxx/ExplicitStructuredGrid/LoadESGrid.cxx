#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkExplicitStructuredGrid.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridToExplicitStructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0]
              << " filename.vtu e.g. The explicit structured grid file name: "
                 "UNISIM-II-D.vtu."
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkXMLUnstructuredGridReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkUnstructuredGridToExplicitStructuredGrid> converter;
  converter->GlobalWarningDisplayOff(); // hide VTK errors
  converter->SetInputConnection(reader->GetOutputPort());
  converter->SetInputArrayToProcess(0, 0, 0, 1, "BLOCK_I");
  converter->SetInputArrayToProcess(1, 0, 0, 1, "BLOCK_J");
  converter->SetInputArrayToProcess(2, 0, 0, 1, "BLOCK_K");
  converter->Update();

  auto grid = converter->GetOutput();
  grid->ComputeFacesConnectivityFlagsArray();
  grid->GetCellData()->SetActiveScalars("ConnectivityFlags");

  auto scalars = grid->GetCellData()->GetArray("ConnectivityFlags");

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputData(grid);
  mapper->SetColorModeToMapScalars();
  mapper->SetScalarRange(scalars->GetRange());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->EdgeVisibilityOn();

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DimGray").GetData());

  vtkNew<vtkRenderWindow> window;
  window->AddRenderer(renderer);
  window->SetWindowName("LoadESGrid");
  window->SetSize(1024, 768);
  window->Render();

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(312452.407650, 7474760.406373, 3507.364723);
  camera->SetFocalPoint(314388.388434, 7481520.509575, -2287.477388);
  camera->SetViewUp(0.089920, 0.633216, 0.768734);
  camera->SetDistance(9111.926908);
  camera->SetClippingRange(595.217338, 19595.429475);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(window);
  vtkNew<vtkInteractorStyleRubberBandPick> style;

  interactor->SetInteractorStyle(style);
  window->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
