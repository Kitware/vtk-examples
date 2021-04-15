#include <vtkActor.h>
#include <vtkCMLMoleculeReader.h>
#include <vtkCamera.h>
#include <vtkMolecule.h>
#include <vtkMoleculeMapper.h>
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
    std::cerr << "Usage: " << argv[0] << " Filename(.cml) e.g. porphyrin.cml"
              << std::endl;
    return EXIT_FAILURE;
  }
  std::string fname(argv[1]);
  vtkNew<vtkCMLMoleculeReader> cmlSource;

  cmlSource->SetFileName(fname.c_str());

  vtkNew<vtkMoleculeMapper> molmapper;
  molmapper->SetInputConnection(cmlSource->GetOutputPort());

  molmapper->UseBallAndStickSettings();

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> actor;
  actor->SetMapper(molmapper);
  actor->GetProperty()->SetDiffuse(0.7);
  actor->GetProperty()->SetSpecular(0.5);
  actor->GetProperty()->SetSpecularPower(20.0);

  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(ren);
  renderWindow->SetWindowName("ReadCML");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renderWindow);

  ren->AddActor(actor);

  renderWindow->SetSize(640, 480);
  renderWindow->Render();
  ren->GetActiveCamera()->Zoom(2.0);
  ren->SetBackground(colors->GetColor3d("Silver").GetData());

  // Finally render the scene
  renderWindow->SetMultiSamples(0);
  renderWindow->GetInteractor()->Initialize();
  renderWindow->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
