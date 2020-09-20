#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricKuen.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>

#include <array>

// These callbacks do the actual work.
// Callbacks for the interactions
class SliderCallbackMinimumU : public vtkCommand
{
public:
  static SliderCallbackMinimumU* New()
  {
    return new SliderCallbackMinimumU;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    if (value > .9 * this->Kuen->GetMaximumU())
    {
      value = .99 * this->Kuen->GetMaximumU();
      static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())
          ->SetValue(value);
    }
    this->Kuen->SetMinimumU(value);
  }
  SliderCallbackMinimumU() : Kuen(0)
  {
  }
  vtkParametricKuen* Kuen;
};

class SliderCallbackMaximumU : public vtkCommand
{
public:
  static SliderCallbackMaximumU* New()
  {
    return new SliderCallbackMaximumU;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    if (value < this->Kuen->GetMinimumU() + .01)
    {
      value = this->Kuen->GetMinimumU() + .01;
      static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())
          ->SetValue(value);
    }
    this->Kuen->SetMaximumU(static_cast<vtkSliderRepresentation2D*>(
                                sliderWidget->GetRepresentation())
                                ->GetValue());
  }
  SliderCallbackMaximumU() : Kuen(0)
  {
  }
  vtkParametricKuen* Kuen;
};

class SliderCallbackMinimumV : public vtkCommand
{
public:
  static SliderCallbackMinimumV* New()
  {
    return new SliderCallbackMinimumV;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    if (value > .9 * this->Kuen->GetMaximumV())
    {
      value = .9 * this->Kuen->GetMaximumV();
      static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())
          ->SetValue(value);
    }
    this->Kuen->SetMinimumV(value);
  }
  SliderCallbackMinimumV() : Kuen(0)
  {
  }
  vtkParametricKuen* Kuen;
};

class SliderCallbackMaximumV : public vtkCommand
{
public:
  static SliderCallbackMaximumV* New()
  {
    return new SliderCallbackMaximumV;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    if (value < this->Kuen->GetMinimumV() + .01)
    {
      value = this->Kuen->GetMinimumV() + .01;
      static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())
          ->SetValue(value);
    }
    this->Kuen->SetMaximumV(static_cast<vtkSliderRepresentation2D*>(
                                sliderWidget->GetRepresentation())
                                ->GetValue());
  }
  SliderCallbackMaximumV() : Kuen(0)
  {
  }
  vtkParametricKuen* Kuen;
};

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Set the background color.
  std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());

  vtkNew<vtkParametricKuen> surface;
  vtkNew<vtkParametricFunctionSource> source;

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkPolyDataMapper> mapper;
  vtkNew<vtkActor> actor;

  vtkNew<vtkProperty> backProperty;
  backProperty->SetColor(colors->GetColor3d("Tomato").GetData());

  // Create a parametric function source, renderer, mapper, and actor
  source->SetParametricFunction(surface);

  mapper->SetInputConnection(source->GetOutputPort());

  actor->SetMapper(mapper);
  actor->SetBackfaceProperty(backProperty);
  actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Banana").GetData());
  actor->GetProperty()->SetSpecular(.5);
  actor->GetProperty()->SetSpecularPower(20);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("ParametricKuenDemo");
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(-30);
  renderer->GetActiveCamera()->Zoom(0.9);
  renderer->ResetCameraClippingRange();

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Setup a slider widget for each varying parameter
  double tubeWidth(.008);
  double sliderLength(.008);
  double titleHeight(.02);
  double labelHeight(.02);

  vtkNew<vtkSliderRepresentation2D> sliderRepMinimumU;

  sliderRepMinimumU->SetMinimumValue(-4.5);
  sliderRepMinimumU->SetMaximumValue(4.5);
  sliderRepMinimumU->SetValue(-4.5);
  sliderRepMinimumU->SetTitleText("U min");

  sliderRepMinimumU->GetPoint1Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  sliderRepMinimumU->GetPoint1Coordinate()->SetValue(.1, .1);
  sliderRepMinimumU->GetPoint2Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  sliderRepMinimumU->GetPoint2Coordinate()->SetValue(.9, .1);

  sliderRepMinimumU->SetTubeWidth(tubeWidth);
  sliderRepMinimumU->SetSliderLength(sliderLength);
  sliderRepMinimumU->SetTitleHeight(titleHeight);
  sliderRepMinimumU->SetLabelHeight(labelHeight);

  vtkNew<vtkSliderWidget> sliderWidgetMinimumU;
  sliderWidgetMinimumU->SetInteractor(interactor);
  sliderWidgetMinimumU->SetRepresentation(sliderRepMinimumU);
  sliderWidgetMinimumU->SetAnimationModeToAnimate();
  sliderWidgetMinimumU->EnabledOn();

  vtkNew<SliderCallbackMinimumU> callbackMinimumU;
  callbackMinimumU->Kuen = surface;

  sliderWidgetMinimumU->AddObserver(vtkCommand::InteractionEvent,
                                    callbackMinimumU);

  vtkNew<vtkSliderRepresentation2D> sliderRepMaximumU;

  sliderRepMaximumU->SetMinimumValue(-4.5);
  sliderRepMaximumU->SetMaximumValue(4.5);
  sliderRepMaximumU->SetValue(4.5);
  sliderRepMaximumU->SetTitleText("U max");

  sliderRepMaximumU->GetPoint1Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  sliderRepMaximumU->GetPoint1Coordinate()->SetValue(.1, .9);
  sliderRepMaximumU->GetPoint2Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  sliderRepMaximumU->GetPoint2Coordinate()->SetValue(.9, .9);

  sliderRepMaximumU->SetTubeWidth(tubeWidth);
  sliderRepMaximumU->SetSliderLength(sliderLength);
  sliderRepMaximumU->SetTitleHeight(titleHeight);
  sliderRepMaximumU->SetLabelHeight(labelHeight);

  vtkNew<vtkSliderWidget> sliderWidgetMaximumU;
  sliderWidgetMaximumU->SetInteractor(interactor);
  sliderWidgetMaximumU->SetRepresentation(sliderRepMaximumU);
  sliderWidgetMaximumU->SetAnimationModeToAnimate();
  sliderWidgetMaximumU->EnabledOn();

  vtkNew<SliderCallbackMaximumU> callbackMaximumU;
  callbackMaximumU->Kuen = surface;

  sliderWidgetMaximumU->AddObserver(vtkCommand::InteractionEvent,
                                    callbackMaximumU);

  vtkNew<vtkSliderRepresentation2D> sliderRepMinimumV;

  sliderRepMinimumV->SetMinimumValue(0.05);
  sliderRepMinimumV->SetMaximumValue(vtkMath::Pi());
  sliderRepMinimumV->SetValue(0.0);
  sliderRepMinimumV->SetTitleText("V min");

  sliderRepMinimumV->GetPoint1Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  sliderRepMinimumV->GetPoint1Coordinate()->SetValue(.1, .1);
  sliderRepMinimumV->GetPoint2Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  sliderRepMinimumV->GetPoint2Coordinate()->SetValue(.1, .9);

  sliderRepMinimumV->SetTubeWidth(tubeWidth);
  sliderRepMinimumV->SetSliderLength(sliderLength);
  sliderRepMinimumV->SetTitleHeight(titleHeight);
  sliderRepMinimumV->SetLabelHeight(labelHeight);

  vtkNew<vtkSliderWidget> sliderWidgetMinimumV;
  sliderWidgetMinimumV->SetInteractor(interactor);
  sliderWidgetMinimumV->SetRepresentation(sliderRepMinimumV);
  sliderWidgetMinimumV->SetAnimationModeToAnimate();
  sliderWidgetMinimumV->EnabledOn();

  vtkNew<SliderCallbackMinimumV> callbackMinimumV;
  callbackMinimumV->Kuen = surface;

  sliderWidgetMinimumV->AddObserver(vtkCommand::InteractionEvent,
                                    callbackMinimumV);

  vtkNew<vtkSliderRepresentation2D> sliderRepMaximumV;

  sliderRepMaximumV->SetMinimumValue(0.05);
  sliderRepMaximumV->SetMaximumValue(vtkMath::Pi() - .05);
  sliderRepMaximumV->SetValue(vtkMath::Pi());
  sliderRepMaximumV->SetTitleText("V max");

  sliderRepMaximumV->GetPoint1Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  sliderRepMaximumV->GetPoint1Coordinate()->SetValue(.9, .1);
  sliderRepMaximumV->GetPoint2Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  sliderRepMaximumV->GetPoint2Coordinate()->SetValue(.9, .9);
  sliderRepMaximumV->SetTubeWidth(tubeWidth);
  sliderRepMaximumV->SetSliderLength(sliderLength);
  sliderRepMaximumV->SetTitleHeight(titleHeight);
  sliderRepMaximumV->SetLabelHeight(labelHeight);

  vtkNew<vtkSliderWidget> sliderWidgetMaximumV;
  sliderWidgetMaximumV->SetInteractor(interactor);
  sliderWidgetMaximumV->SetRepresentation(sliderRepMaximumV);
  sliderWidgetMaximumV->SetAnimationModeToAnimate();
  sliderWidgetMaximumV->EnabledOn();

  vtkNew<SliderCallbackMaximumV> callbackMaximumV;
  callbackMaximumV->Kuen = surface;

  sliderWidgetMaximumV->AddObserver(vtkCommand::InteractionEvent,
                                    callbackMaximumV);

  surface->SetMinimumU(-4.5);
  surface->SetMaximumU(4.5);
  surface->SetMinimumV(0.05);
  surface->SetMaximumV(vtkMath::Pi() - .05);

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(-30);
  renderer->GetActiveCamera()->Zoom(0.9);
  renderer->ResetCameraClippingRange();
  renderWindow->Render();

  interactor->Initialize();

  interactor->Start();

  return EXIT_SUCCESS;
}
