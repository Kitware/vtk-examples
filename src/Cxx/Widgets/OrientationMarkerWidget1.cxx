#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSuperquadricSource.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  // Parse command line arguments
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " Filename(.vtp) e.g. Bunny.vtp"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  // Read the polydata for the icon
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(argv[1]);

  vtkNew<vtkDataSetMapper> iconMapper;
  iconMapper->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkActor> iconActor;
  iconActor->SetMapper(iconMapper);
  iconActor->GetProperty()->SetColor(colors->GetColor3d("Silver").GetData());

  // Set up the renderer, window, and interactor
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  renWin->SetSize(400, 400);
  renWin->SetWindowName("OrientationMarkerWidget1");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  double r;
  double g;
  double b;
  colors->GetColor("Wheat", r, g, b);
  // Set up the widget
  vtkNew<vtkOrientationMarkerWidget> widget;
  widget->SetOrientationMarker(iconActor);
  widget->SetInteractor(iren);
  widget->SetViewport(0.0, 0.0, 0.2, 0.2);
  widget->SetOutlineColor(r,g,b);
  widget->SetEnabled(1);
  widget->InteractiveOn();

  // Create a superquadric
  vtkNew<vtkSuperquadricSource> superquadricSource;
  superquadricSource->SetPhiRoundness(0.2);
  superquadricSource->SetThetaRoundness(0.8);

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> superquadricMapper;
  superquadricMapper->SetInputConnection(superquadricSource->GetOutputPort());

  vtkNew<vtkActor> superquadricActor;
  superquadricActor->SetMapper(superquadricMapper);
  superquadricActor->GetProperty()->SetInterpolationToFlat();
  superquadricActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Carrot").GetData());
  superquadricActor->GetProperty()->SetSpecularColor(
      colors->GetColor3d("White").GetData());
  superquadricActor->GetProperty()->SetDiffuse(.6);
  superquadricActor->GetProperty()->SetSpecular(.5);
  superquadricActor->GetProperty()->SetSpecularPower(5.0);

  renderer->AddActor(superquadricActor);
  renderer->ResetCamera();

  renWin->Render();

  iren->Initialize();

  iren->Start();

  return EXIT_SUCCESS;
}
