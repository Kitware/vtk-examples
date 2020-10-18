#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCommand.h>
#include <vtkGlyph3DMapper.h>
#include <vtkKochanekSpline.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <vtkSphereSource.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

namespace {
void MakeTensionWidget(vtkSliderWidget*, vtkParametricFunctionSource*,
                       vtkParametricSpline*, vtkRenderer*,
                       vtkRenderWindowInteractor*);
void MakeContinuityWidget(vtkSliderWidget*, vtkParametricFunctionSource*,
                          vtkParametricSpline*, vtkRenderer*,
                          vtkRenderWindowInteractor*);
void MakeBiasWidget(vtkSliderWidget*, vtkParametricFunctionSource*,
                    vtkParametricSpline*, vtkRenderer*,
                    vtkRenderWindowInteractor*);
void SetSliderColors(vtkSliderRepresentation2D* slider);
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  int numberOfPoints = 7;
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(numberOfPoints);
  double radius = 1.0;
  double delta = 2.0 * vtkMath::Pi() / double(numberOfPoints);
  for (int n = 0; n < numberOfPoints; ++n)
  {
    double theta = delta * n;
    double x = radius * std::cos(theta);
    double y = radius * std::sin(theta);
    double z = n * (vtkMath::Pi() / numberOfPoints);
    points->SetPoint(n, x, y, z);
  }

  vtkNew<vtkKochanekSpline> xSpline;
  vtkNew<vtkKochanekSpline> ySpline;
  vtkNew<vtkKochanekSpline> zSpline;

  vtkNew<vtkParametricSpline> spline;
  spline->SetXSpline(xSpline);
  spline->SetYSpline(ySpline);
  spline->SetZSpline(zSpline);
  spline->SetPoints(points);

  vtkNew<vtkParametricFunctionSource> functionSource;
  functionSource->SetParametricFunction(spline);
  functionSource->SetUResolution(50 * numberOfPoints);
  functionSource->SetVResolution(50 * numberOfPoints);
  functionSource->SetWResolution(50 * numberOfPoints);
  functionSource->Update();

  // Setup actor and mapper
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(functionSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("DarkSlateGrey").GetData());
  actor->GetProperty()->SetLineWidth(3.0);

  // Glyph the points
  vtkNew<vtkSphereSource> sphere;
  sphere->SetPhiResolution(21);
  sphere->SetThetaResolution(21);
  sphere->SetRadius(.1);

  // Create a polydata to store everything in
  vtkNew<vtkPolyData> polyData;
  polyData->SetPoints(points);

  vtkNew<vtkGlyph3DMapper> pointMapper;
  pointMapper->SetInputData(polyData);
  pointMapper->SetSourceConnection(sphere->GetOutputPort());

  vtkNew<vtkActor> pointActor;
  pointActor->SetMapper(pointMapper);
  pointActor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());
  pointActor->GetProperty()->SetOpacity(.5);
  ;

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("KochanekSpline");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->AddActor(pointActor);
  ;
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());
  renderWindow->Render();

  vtkNew<vtkSliderWidget> tensionWidget;
  MakeTensionWidget(tensionWidget, functionSource, spline, renderer,
                    renderWindowInteractor);
  vtkNew<vtkSliderWidget> continuityWidget;
  MakeContinuityWidget(continuityWidget, functionSource, spline, renderer,
                       renderWindowInteractor);
  vtkNew<vtkSliderWidget> biasWidget;
  MakeBiasWidget(biasWidget, functionSource, spline, renderer,
                 renderWindowInteractor);

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
// These callbacks do the actual work.
// Callbacks for the interactions
class SliderCallbackTension : public vtkCommand
{
public:
  static SliderCallbackTension* New()
  {
    return new SliderCallbackTension;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    dynamic_cast<vtkKochanekSpline*>(this->ParametricSpline->GetXSpline())
        ->SetDefaultTension(value);
    dynamic_cast<vtkKochanekSpline*>(this->ParametricSpline->GetYSpline())
        ->SetDefaultTension(value);
    dynamic_cast<vtkKochanekSpline*>(this->ParametricSpline->GetZSpline())
        ->SetDefaultTension(value);
    ParametricSource->Modified();
    ParametricSource->Update();
  }
  SliderCallbackTension() : ParametricSource(0), ParametricSpline(0)
  {
  }
  vtkParametricSpline* ParametricSpline;
  vtkParametricFunctionSource* ParametricSource;
};

void MakeTensionWidget(vtkSliderWidget* widget,
                       vtkParametricFunctionSource* parametricSource,
                       vtkParametricSpline* parametricSpline,
                       vtkRenderer* renderer,
                       vtkRenderWindowInteractor* interactor)
{
  // Setup a slider widget for each varying parameter
  double tubeWidth(.005);
  double sliderLength(.02);
  double titleHeight(.02);
  double labelHeight(.02);

  vtkNew<vtkSliderRepresentation2D> sliderRepTension;

  sliderRepTension->SetRenderer(renderer);

  sliderRepTension->SetMinimumValue(-1.0);
  sliderRepTension->SetMaximumValue(1.0);
  sliderRepTension->SetValue(
      dynamic_cast<vtkKochanekSpline*>(parametricSpline->GetXSpline())
          ->GetDefaultTension());
  sliderRepTension->SetTitleText("Tension");
  sliderRepTension->SetRenderer(renderer);
  sliderRepTension->GetPoint1Coordinate()->SetValue(0.1, 0.1);
  sliderRepTension->GetPoint1Coordinate()
      ->SetCoordinateSystemToNormalizedViewport();
  sliderRepTension->GetPoint2Coordinate()->SetValue(0.3, 0.1);
  sliderRepTension->GetPoint2Coordinate()
      ->SetCoordinateSystemToNormalizedViewport();

  sliderRepTension->SetTubeWidth(tubeWidth);
  sliderRepTension->SetSliderLength(sliderLength);
  sliderRepTension->SetTitleHeight(titleHeight);
  sliderRepTension->SetLabelHeight(labelHeight);
  sliderRepTension->SetEndCapLength(tubeWidth * 1.5);
  sliderRepTension->SetSliderWidth(tubeWidth * 1.5);
  sliderRepTension->BuildRepresentation();
  SetSliderColors(sliderRepTension);

  widget->SetRepresentation(sliderRepTension);
  widget->SetAnimationModeToAnimate();
  widget->SetNumberOfAnimationSteps(10);
  widget->SetInteractor(interactor);
  widget->EnabledOn();

  vtkNew<SliderCallbackTension> callbackTension;
  callbackTension->ParametricSpline = parametricSpline;
  callbackTension->ParametricSource = parametricSource;

  widget->AddObserver(vtkCommand::InteractionEvent, callbackTension);
}

class SliderCallbackContinuity : public vtkCommand
{
public:
  static SliderCallbackContinuity* New()
  {
    return new SliderCallbackContinuity;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    dynamic_cast<vtkKochanekSpline*>(this->ParametricSpline->GetXSpline())
        ->SetDefaultContinuity(value);
    dynamic_cast<vtkKochanekSpline*>(this->ParametricSpline->GetYSpline())
        ->SetDefaultContinuity(value);
    dynamic_cast<vtkKochanekSpline*>(this->ParametricSpline->GetZSpline())
        ->SetDefaultContinuity(value);
    ParametricSource->Modified();
    ParametricSource->Update();
  }
  SliderCallbackContinuity() : ParametricSource(0), ParametricSpline(0)
  {
  }
  vtkParametricSpline* ParametricSpline;
  vtkParametricFunctionSource* ParametricSource;
};

void MakeContinuityWidget(vtkSliderWidget* widget,
                          vtkParametricFunctionSource* parametricSource,
                          vtkParametricSpline* parametricSpline,
                          vtkRenderer* renderer,
                          vtkRenderWindowInteractor* interactor)
{
  // Setup a slider widget for each varying parameter
  double tubeWidth(.005);
  double sliderLength(.02);
  double titleHeight(.02);
  double labelHeight(.02);

  vtkNew<vtkSliderRepresentation2D> sliderRepContinuity;

  sliderRepContinuity->SetRenderer(renderer);

  sliderRepContinuity->SetMinimumValue(-1.0);
  sliderRepContinuity->SetMaximumValue(1.0);
  sliderRepContinuity->SetValue(
      dynamic_cast<vtkKochanekSpline*>(parametricSpline->GetXSpline())
          ->GetDefaultContinuity());
  sliderRepContinuity->SetTitleText("Continuity");
  sliderRepContinuity->SetRenderer(renderer);
  sliderRepContinuity->GetPoint1Coordinate()->SetValue(0.4, 0.1);
  sliderRepContinuity->GetPoint1Coordinate()
      ->SetCoordinateSystemToNormalizedViewport();
  sliderRepContinuity->GetPoint2Coordinate()->SetValue(0.6, 0.1);
  sliderRepContinuity->GetPoint2Coordinate()
      ->SetCoordinateSystemToNormalizedViewport();

  sliderRepContinuity->SetTubeWidth(tubeWidth);
  sliderRepContinuity->SetSliderLength(sliderLength);
  sliderRepContinuity->SetTitleHeight(titleHeight);
  sliderRepContinuity->SetLabelHeight(labelHeight);
  sliderRepContinuity->SetEndCapLength(tubeWidth * 1.5);
  sliderRepContinuity->SetSliderWidth(tubeWidth * 1.5);
  sliderRepContinuity->BuildRepresentation();
  SetSliderColors(sliderRepContinuity);

  widget->SetRepresentation(sliderRepContinuity);
  widget->SetAnimationModeToAnimate();
  widget->SetNumberOfAnimationSteps(10);
  widget->SetInteractor(interactor);
  widget->EnabledOn();

  vtkNew<SliderCallbackContinuity> callbackContinuity;
  callbackContinuity->ParametricSpline = parametricSpline;
  callbackContinuity->ParametricSource = parametricSource;

  widget->AddObserver(vtkCommand::InteractionEvent, callbackContinuity);
}

class SliderCallbackBias : public vtkCommand
{
public:
  static SliderCallbackBias* New()
  {
    return new SliderCallbackBias;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    dynamic_cast<vtkKochanekSpline*>(this->ParametricSpline->GetXSpline())
        ->SetDefaultBias(value);
    dynamic_cast<vtkKochanekSpline*>(this->ParametricSpline->GetYSpline())
        ->SetDefaultBias(value);
    dynamic_cast<vtkKochanekSpline*>(this->ParametricSpline->GetZSpline())
        ->SetDefaultBias(value);
    ParametricSource->Modified();
    ParametricSource->Update();
  }
  SliderCallbackBias() : ParametricSource(0), ParametricSpline(0)
  {
  }
  vtkParametricSpline* ParametricSpline;
  vtkParametricFunctionSource* ParametricSource;
};

void MakeBiasWidget(vtkSliderWidget* widget,
                    vtkParametricFunctionSource* parametricSource,
                    vtkParametricSpline* parametricSpline,
                    vtkRenderer* renderer,
                    vtkRenderWindowInteractor* interactor)
{
  // Setup a slider widget for each varying parameter
  double tubeWidth(.005);
  double sliderLength(.02);
  double titleHeight(.02);
  double labelHeight(.02);

  vtkNew<vtkSliderRepresentation2D> sliderRepBias;

  sliderRepBias->SetRenderer(renderer);

  sliderRepBias->SetMinimumValue(-1.0);
  sliderRepBias->SetMaximumValue(1.0);
  sliderRepBias->SetValue(
      dynamic_cast<vtkKochanekSpline*>(parametricSpline->GetXSpline())
          ->GetDefaultBias());
  sliderRepBias->SetTitleText("Bias");
  sliderRepBias->SetRenderer(renderer);
  sliderRepBias->GetPoint1Coordinate()->SetValue(0.7, 0.1);
  sliderRepBias->GetPoint1Coordinate()
      ->SetCoordinateSystemToNormalizedViewport();
  sliderRepBias->GetPoint2Coordinate()->SetValue(0.9, 0.1);
  sliderRepBias->GetPoint2Coordinate()
      ->SetCoordinateSystemToNormalizedViewport();

  sliderRepBias->SetTubeWidth(tubeWidth);
  sliderRepBias->SetSliderLength(sliderLength);
  sliderRepBias->SetTitleHeight(titleHeight);
  sliderRepBias->SetLabelHeight(labelHeight);
  sliderRepBias->SetEndCapLength(tubeWidth * 1.5);
  sliderRepBias->SetSliderWidth(tubeWidth * 1.5);
  sliderRepBias->BuildRepresentation();
  SetSliderColors(sliderRepBias);

  widget->SetRepresentation(sliderRepBias);
  widget->SetAnimationModeToAnimate();
  widget->SetNumberOfAnimationSteps(10);
  widget->SetInteractor(interactor);
  widget->EnabledOn();

  vtkNew<SliderCallbackBias> callbackBias;
  callbackBias->ParametricSpline = parametricSpline;
  callbackBias->ParametricSource = parametricSource;

  widget->AddObserver(vtkCommand::InteractionEvent, callbackBias);
}

void SetSliderColors(vtkSliderRepresentation2D* slider)
{
  vtkNew<vtkNamedColors> colors;
  // Set color properties:
  // Change the color of the knob that slides
  slider->GetSliderProperty()->SetColor(colors->GetColor3d("Peru").GetData());
  // Change the color of the text indicating what the slider controls
  slider->GetTitleProperty()->SetColor(
      colors->GetColor3d("OrangeRed").GetData());
  // Change the color of the text displaying the value
  slider->GetLabelProperty()->SetColor(
      colors->GetColor3d("OrangeRed").GetData());
  // Change the color of the knob when the mouse is held on it
  slider->GetSelectedProperty()->SetColor(
      colors->GetColor3d("DeepPink").GetData());
  // Change the color of the bar
  slider->GetTubeProperty()->SetColor(colors->GetColor3d("Gold").GetData());
  // Change the color of the ends of the bar
  slider->GetCapProperty()->SetColor(colors->GetColor3d("Gold").GetData());
}

} // namespace
