#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataSilhouette.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {
// Handle mouse events
class MouseInteractorHighLightActor : public vtkInteractorStyleTrackballCamera
{
public:
  static MouseInteractorHighLightActor* New();
  vtkTypeMacro(MouseInteractorHighLightActor,
               vtkInteractorStyleTrackballCamera);

  MouseInteractorHighLightActor()
  {
    LastPickedActor = nullptr;
    SilhouetteActor = nullptr;
    Silhouette = nullptr;
  }
  virtual ~MouseInteractorHighLightActor()
  {
  }
  virtual void OnLeftButtonDown() override
  {
    int* clickPos = this->GetInteractor()->GetEventPosition();

    // Pick from this location.
    vtkNew<vtkPropPicker> picker;
    picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());
    this->LastPickedActor = picker->GetActor();

    // If we picked something before, remove the silhouette actor and
    // generate a new one
    if (this->LastPickedActor)
    {
      this->GetDefaultRenderer()->RemoveActor(this->SilhouetteActor);

      // Highlight the picked actor by generating a silouhette
      this->Silhouette->SetInputData(
          dynamic_cast<vtkPolyDataMapper*>(this->LastPickedActor->GetMapper())
              ->GetInput());
      this->GetDefaultRenderer()->AddActor(this->SilhouetteActor);
    }

    // Forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  }
  void SetSilhouette(vtkPolyDataSilhouette* silhouette)
  {
    this->Silhouette = silhouette;
  }
  void SetSilhouetteActor(vtkActor* silhouetteActor)
  {
    this->SilhouetteActor = silhouetteActor;
  }

private:
  vtkActor* LastPickedActor;
  vtkActor* SilhouetteActor;
  vtkPolyDataSilhouette* Silhouette;
};

vtkStandardNewMacro(MouseInteractorHighLightActor);
} // namespace

// Execute application.
int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;
  colors->SetColor("Bkg", 0.3, 0.4, 0.5);

  int numberOfSpheres = 10;
  if (argc > 1)
  {
    numberOfSpheres = atoi(argv[1]);
  }
  // Create a renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 480);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("HighlightWithSilhouette");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);
  for (int i = 0; i < numberOfSpheres; ++i)
  {
    vtkNew<vtkSphereSource> source;
    double x, y, z, radius;
    // random position and radius
    x = randomSequence->GetRangeValue(-5.0, 5.0);
    randomSequence->Next();
    y = randomSequence->GetRangeValue(-5.0, 5.0);
    randomSequence->Next();
    z = randomSequence->GetRangeValue(-5.0, 5.0);
    randomSequence->Next();
    radius = randomSequence->GetRangeValue(0.5, 1.0);
    randomSequence->Next();
    source->SetRadius(radius);
    source->SetCenter(x, y, z);
    source->SetPhiResolution(11);
    source->SetThetaResolution(21);
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(source->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    double r, g, b;
    r = randomSequence->GetRangeValue(0.4, 1.0);
    randomSequence->Next();
    g = randomSequence->GetRangeValue(0.4, 1.0);
    randomSequence->Next();
    b = randomSequence->GetRangeValue(0.4, 1.0);
    randomSequence->Next();
    actor->GetProperty()->SetDiffuseColor(r, g, b);
    actor->GetProperty()->SetDiffuse(0.8);
    actor->GetProperty()->SetSpecular(0.5);
    actor->GetProperty()->SetSpecularColor(
        colors->GetColor3d("White").GetData());
    actor->GetProperty()->SetSpecularPower(30.0);
    renderer->AddActor(actor);
  }

  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  // Render and interact
  renderWindow->Render();

  // Create the silhouette pipeline, the input data will be set in the
  // interactor
  vtkNew<vtkPolyDataSilhouette> silhouette;
  silhouette->SetCamera(renderer->GetActiveCamera());

  // Create mapper and actor for silhouette
  vtkNew<vtkPolyDataMapper> silhouetteMapper;
  silhouetteMapper->SetInputConnection(silhouette->GetOutputPort());

  vtkNew<vtkActor> silhouetteActor;
  silhouetteActor->SetMapper(silhouetteMapper);
  silhouetteActor->GetProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());
  silhouetteActor->GetProperty()->SetLineWidth(5);

  // Set the custom type to use for interaction.
  vtkNew<MouseInteractorHighLightActor> style;
  style->SetDefaultRenderer(renderer);
  style->SetSilhouetteActor(silhouetteActor);
  style->SetSilhouette(silhouette);

  interactor->SetInteractorStyle(style);

  interactor->Start();

  return EXIT_SUCCESS;
}
