#include <vtkActor.h>
#include <vtkCameraOrientationWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Demonstrates a 3D camera orientation widget." << std::endl;
    std::cout << "Usage: " << argv[0] << " fileName" << std::endl;
    std::cout << "where: fileName is cow.vtp." << std::endl;
    return EXIT_FAILURE;
  }
  std::string fileName(argv[1]);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iRen;

  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(fileName.c_str());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->GetProperty()->SetColor(colors->GetColor3d("Beige").GetData());
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DimGray").GetData());

  renWin->AddRenderer(renderer);
  renWin->SetSize(600, 600);
  renWin->SetWindowName("CameraOrientationWidget");

  // Important: The interactor must be set prior to enabling the widget.
  iRen->SetRenderWindow(renWin);

  vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
  camOrientManipulator->SetParentRenderer(renderer);
  // Enable the widget.
  camOrientManipulator->On();

  renWin->Render();
  iRen->Initialize();
  iRen->Start();

  return EXIT_SUCCESS;
}
