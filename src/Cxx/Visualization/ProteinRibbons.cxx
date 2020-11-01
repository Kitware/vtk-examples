#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPDBReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProteinRibbonFilter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.pdb e.g. 2j6g.pdb" << std::endl;
    return EXIT_FAILURE;
  }
  std::string fileName(argv[1]);

  // read protein from pdb
  vtkNew<vtkPDBReader> reader;
  reader->SetFileName(fileName.c_str());

  // setup ribbon filter
  vtkNew<vtkProteinRibbonFilter> ribbonFilter;
  ribbonFilter->SetInputConnection(reader->GetOutputPort());

  // setup poly data mapper
  vtkNew<vtkPolyDataMapper> polyDataMapper;
  polyDataMapper->SetInputConnection(ribbonFilter->GetOutputPort());

  // setup actor
  vtkNew<vtkActor> actor;
  actor->SetMapper(polyDataMapper);
  actor->GetProperty()->SetDiffuse(.7);
  actor->GetProperty()->SetSpecular(.5);
  actor->GetProperty()->SetSpecularPower(80.0);

  // setup render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);
  vtkInteractorStyleSwitch* style =
      dynamic_cast<vtkInteractorStyleSwitch*>(interactor->GetInteractorStyle());
  style->SetCurrentStyleToTrackballCamera();

  vtkNew<vtkNamedColors> colors;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("ProteinRibbons");

  renderWindow->Render();
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(45);
  renderer->GetActiveCamera()->Elevation(45);
  renderer->GetActiveCamera()->Zoom(1.7);
  renderer->ResetCameraClippingRange();
  renderWindow->Render();

  // Finally render the scene
  renderWindow->SetMultiSamples(0);
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
