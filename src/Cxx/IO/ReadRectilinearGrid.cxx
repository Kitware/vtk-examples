#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRectilinearGridGeometryFilter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXMLRectilinearGridReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify input arguments
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0]
              << " Filename.vtr e.g. RectilinearGrid.vtr" << std::endl;
    return EXIT_FAILURE;
  }

  // Parse arguments
  std::string inputFilename = argv[1];

  // Read the file
  vtkNew<vtkXMLRectilinearGridReader> reader;
  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  // vtkNew<vtkRectilinearGridGeometryFilter> geometryFilter;
  // geometryFilter->SetInputConnection(reader->GetOutputPort());
  // geometryFilter->Update();

  // Visualize
  vtkNew<vtkDataSetMapper> mapper;
  // mapper->SetInputConnection(geometryFilter->GetOutputPort());
  mapper->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetRepresentationToWireframe();

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ReadRectilinearGrid");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("LightSteelBlue").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
