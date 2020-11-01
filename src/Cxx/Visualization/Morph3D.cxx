#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkImplicitModeller.h>
#include <vtkInterpolateDataSetAttributes.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVectorText.h>

#include <vtksys/SystemTools.hxx>

int main(int argc, char* argv[])
{
  double t = 2.0;
  if (argc > 1)
  {
    t = atof(argv[1]);
  }

  // make the letter v
  vtkNew<vtkVectorText> letterV;
  letterV->SetText("v");

  // read the geometry file containing the letter t
  vtkNew<vtkVectorText> letterT;
  letterT->SetText("t");

  // read the geometry file containing the letter k
  vtkNew<vtkVectorText> letterK;
  letterK->SetText("k");

  // create implicit models of each letter
  vtkNew<vtkImplicitModeller> blobbyV;
  blobbyV->SetInputConnection(letterV->GetOutputPort());
  blobbyV->SetMaximumDistance(.2);
  blobbyV->SetSampleDimensions(50, 50, 12);
  blobbyV->SetModelBounds(-0.5, 1.5, -0.5, 1.5, -0.5, 0.5);

  vtkNew<vtkImplicitModeller> blobbyT;
  blobbyT->SetInputConnection(letterT->GetOutputPort());
  blobbyT->SetMaximumDistance(.2);
  blobbyT->SetSampleDimensions(50, 50, 12);
  blobbyT->SetModelBounds(-0.5, 1.5, -0.5, 1.5, -0.5, 0.5);

  vtkNew<vtkImplicitModeller> blobbyK;
  blobbyK->SetInputConnection(letterK->GetOutputPort());
  blobbyK->SetMaximumDistance(.2);
  blobbyK->SetSampleDimensions(50, 50, 12);
  blobbyK->SetModelBounds(-0.5, 1.5, -0.5, 1.5, -0.5, 0.5);

  // Interpolate the data
  vtkNew<vtkInterpolateDataSetAttributes> interpolate;
  interpolate->AddInputConnection(blobbyV->GetOutputPort());
  interpolate->AddInputConnection(blobbyT->GetOutputPort());
  interpolate->AddInputConnection(blobbyK->GetOutputPort());
  interpolate->SetT(0.0);

  // extract an iso surface
  vtkNew<vtkContourFilter> blobbyIso;
  blobbyIso->SetInputConnection(interpolate->GetOutputPort());
  blobbyIso->SetValue(0, 0.1);

  // map to rendering primitives
  vtkNew<vtkPolyDataMapper> blobbyMapper;
  blobbyMapper->SetInputConnection(blobbyIso->GetOutputPort());
  blobbyMapper->ScalarVisibilityOff();

  vtkNew<vtkNamedColors> colors;

  // now an actor
  vtkNew<vtkActor> blobby;
  blobby->SetMapper(blobbyMapper);
  blobby->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Banana").GetData());
  blobby->GetProperty()->EdgeVisibilityOn();

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkCamera> camera;
  camera->SetClippingRange(0.265, 13.2);
  camera->SetFocalPoint(0.539, 0.47464, 0);
  camera->SetPosition(0.539, 0.474674, 2.644);
  camera->SetViewUp(0, 1, 0);
  camera->Azimuth(30.);
  camera->Elevation(30.);
  renderer->SetActiveCamera(camera);

  //  now  make a renderer and tell it about lights and actors
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("Morph3D");

  renderer->AddActor(blobby);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderWindow->Render();
  for (unsigned i = 0; i < 100; ++i)
  {
    interpolate->SetT(i / 99.0 * 2.0);
    interpolate->Modified();
    renderWindow->Render();
    vtksys::SystemTools::Delay(100);
  }
  interpolate->SetT(t);
  renderWindow->Render();

  // interact
  interactor->Start();
  return EXIT_SUCCESS;
}
