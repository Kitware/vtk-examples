#include <vtkCamera.h>
#include <vtkGLTFImporter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLight.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  if (argc <= 1)
  {
    std::cout << "Usage: " << argv[0] << " <gltf file> e.g. FlightHelmet.gltf"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  vtkColor3d backgroundColor = colors->GetColor3d("SlateGray");

  vtkNew<vtkGLTFImporter> importer;
  importer->SetFileName(argv[1]);

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(backgroundColor.GetData());
  renderer->UseHiddenLineRemovalOn();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 512);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("GLTFImporter");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style);

  importer->SetRenderWindow(renderWindow);
  importer->Update();

  vtkNew<vtkLight> headLight;
  headLight->SetLightTypeToHeadlight();
  headLight->SwitchOn();
  renderer->AddLight(headLight);

  renderWindow->Render();
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(20);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCameraClippingRange();

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
