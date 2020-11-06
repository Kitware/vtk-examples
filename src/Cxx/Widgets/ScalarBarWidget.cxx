#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Parse command line arguments
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " Filename(.vtk) e.g.uGridEx.vtk"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Create a custom lut. The lut is used for both the mapper and the
  // scalarBar
  vtkNew<vtkLookupTable> lut;
  lut->Build();

  // Read the source file.
  vtkNew<vtkUnstructuredGridReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update(); // Needed because of GetScalarRange
  auto output = reader->GetOutput();
  auto scalarRange = output->GetScalarRange();

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputData(output);
  mapper->SetScalarRange(scalarRange);
  mapper->SetLookupTable(lut);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(300, 300);
  renderWindow->SetWindowName("ScalarBarWidget");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // create the scalarBar
  vtkNew<vtkScalarBarActor> scalarBar;
  scalarBar->SetOrientationToHorizontal();
  scalarBar->SetLookupTable(lut);

  // create the scalarBarWidget
  vtkNew<vtkScalarBarWidget> scalarBarWidget;
  scalarBarWidget->SetInteractor(interactor);
  scalarBarWidget->SetScalarBarActor(scalarBar);
  scalarBarWidget->On();

  interactor->Initialize();
  renderWindow->Render();
  renderer->GetActiveCamera()->SetPosition(-6.4, 10.3, 1.4);
  renderer->GetActiveCamera()->SetFocalPoint(1.0, 0.5, 3.0);
  renderer->GetActiveCamera()->SetViewUp(0.6, 0.4, -0.7);
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
