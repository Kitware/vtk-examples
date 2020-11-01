#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cout << "Required parameters: Filename e.g. Ring.vtp" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  std::string inputFilename = argv[1];

  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(inputFilename.c_str());
  reader->Update();
  vtkPolyData* polydata = reader->GetOutput();
  // vtkPoints* Points = Polydata->GetPoints();

  vtkNew<vtkPolyDataMapper2D> mapper;
  mapper->SetInputData(polydata);
  mapper->ScalarVisibilityOff();
  vtkNew<vtkActor2D> actor;
  actor->SetMapper(mapper);

  vtkProperty2D* property2D = actor->GetProperty();
  property2D->SetColor(colors->GetColor3d("Gold").GetData());
  property2D->SetPointSize(2.0);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  // Renderer and RenderWindow
  renderer->ResetCamera();
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("Visualize2DPoints");

  renderWindow->SetSize(200, 200);
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
