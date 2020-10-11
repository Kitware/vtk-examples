#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
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
    LastPickedActor = NULL;
    LastPickedProperty = vtkProperty::New();
  }
  virtual ~MouseInteractorHighLightActor()
  {
    LastPickedProperty->Delete();
  }
  virtual void OnLeftButtonDown() override
  {
    vtkNew<vtkNamedColors> colors;

    int* clickPos = this->GetInteractor()->GetEventPosition();

    // Pick from this location.
    vtkNew<vtkPropPicker> picker;
    picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

    // If we picked something before, reset its property
    if (this->LastPickedActor)
    {
      this->LastPickedActor->GetProperty()->DeepCopy(this->LastPickedProperty);
    }
    this->LastPickedActor = picker->GetActor();
    if (this->LastPickedActor)
    {
      // Save the property of the picked actor so that we can
      // restore it next time
      this->LastPickedProperty->DeepCopy(this->LastPickedActor->GetProperty());
      // Highlight the picked actor by changing its properties
      this->LastPickedActor->GetProperty()->SetColor(
          colors->GetColor3d("Red").GetData());
      this->LastPickedActor->GetProperty()->SetDiffuse(1.0);
      this->LastPickedActor->GetProperty()->SetSpecular(0.0);
      this->LastPickedActor->GetProperty()->EdgeVisibilityOn();
    }

    // Forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  }

private:
  vtkActor* LastPickedActor;
  vtkProperty* LastPickedProperty;
};

vtkStandardNewMacro(MouseInteractorHighLightActor);
} // namespace

// Execute application.
int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  int numberOfSpheres = 10;
  if (argc > 1)
  {
    numberOfSpheres = atoi(argv[1]);
  }
  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 480);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("HighlightPickedActor");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Set the custom type to use for interaction.
  vtkNew<MouseInteractorHighLightActor> style;
  style->SetDefaultRenderer(renderer);

  renderWindowInteractor->SetInteractorStyle(style);
  
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
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
