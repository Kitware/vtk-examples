#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVRMLImporter.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Required arguments: Filename e.g. sextant.wrl" << std::endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];
  std::cout << "Reading " << filename << std::endl;

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("VRMLImporter");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // VRML Import
  vtkNew<vtkVRMLImporter> importer;
  importer->SetFileName(filename.c_str());
  importer->SetRenderWindow(renderWindow);
  importer->Update();

  auto actors = renderer->GetActors();
  std::cout << "There are " << actors->GetNumberOfItems() << " actors"
            << std::endl;

  renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());
  renderWindow->SetSize(512, 512);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
