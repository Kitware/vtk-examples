#include <vtk3DSImporter.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.3ds e.g. iflamingo.3ds"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtk3DSImporter> importer;
  importer->SetFileName(argv[1]);
  importer->ComputeNormalsOn();

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iren;

  renWin->AddRenderer(renderer);
  renderer->SetBackground2(colors->GetColor3d("Gold").GetData());
  renderer->SetBackground(colors->GetColor3d("Wheat").GetData());
  renderer->GradientBackgroundOn();

  iren->SetRenderWindow(renWin);
  importer->SetRenderWindow(renWin);
  importer->Update();

  auto actors = renderer->GetActors();
  std::cout << "There are " << actors->GetNumberOfItems() << " actors."
            << std::endl;

  renWin->SetWindowName("3DSImporter");

  renWin->Render();

  vtkNew<vtkCamera> camera;
  camera->SetPosition(0, -1, 0);
  camera->SetFocalPoint(0, 0, 0);
  camera->SetViewUp(0, 0, 1);
  camera->Azimuth(150);
  camera->Elevation(30);

  renderer->SetActiveCamera(camera);
  renderer->ResetCamera();
  renderer->ResetCameraClippingRange();

  iren->Start();

  return EXIT_SUCCESS;
}
