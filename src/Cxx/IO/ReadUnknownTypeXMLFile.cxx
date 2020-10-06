#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLGenericDataObjectReader.h>

int main(int argc, char* argv[])
{
  // Ensure a filename was specified
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " InputFilename" << std::endl;
    return EXIT_FAILURE;
  }

  // Get all data from the file
  vtkNew<vtkXMLGenericDataObjectReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  // All of the standard data types can be checked and obtained like this:
  if (dynamic_cast<vtkPolyData*>(reader->GetOutput()))
  {
    std::cout << "File is a polydata" << std::endl;
  }
  else if (dynamic_cast<vtkUnstructuredGrid*>(reader->GetOutput()))
  {
    std::cout << "File is an unstructured grid" << std::endl;
  }

  // Visualize
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Moccasin").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ReadUnknownTypeXMLFile");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderWindow->SetSize(640, 480);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
