#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkContourFilter.h>
#include <vtkGenericOutlineFilter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkStreamTracer.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVRMLImporter.h>
#include <vtkXMLUnstructuredGridReader.h>

#include <array>
#include <chrono>
#include <thread>

namespace {
void MakeXWidget(vtkSliderWidget*, vtkPointSource*, vtkSphereSource*,
                 vtkRenderer*, vtkRenderWindowInteractor*);
void MakeYWidget(vtkSliderWidget*, vtkPointSource*, vtkSphereSource*,
                 vtkRenderer*, vtkRenderWindowInteractor*);
void MakeZWidget(vtkSliderWidget*, vtkPointSource*, vtkSphereSource*,
                 vtkRenderer*, vtkRenderWindowInteractor*);
} // namespace

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0]
              << "  geometry.wrl velocity.vtu e.g. room_vis.wrl fire_ug.vtu"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;
  vtkColor3d isoSurfaceColor = colors->GetColor3d("WhiteSmoke");
  vtkColor3d sphereColor = colors->GetColor3d("hotpink");
  vtkColor3d backgroundColor = colors->GetColor3d("SlateGray");

  vtkNew<vtkRenderer> renderer;
  renderer->UseHiddenLineRemovalOn();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Import the VRML Files that define the geometry
  vtkNew<vtkVRMLImporter> vrmlImport;
  vrmlImport->SetRenderWindow(renderWindow);
  vrmlImport->SetFileName(argv[1]);
  vrmlImport->Update();

  // Read the UnstructuredGrid define the solution
  vtkNew<vtkXMLUnstructuredGridReader> solution;
  solution->SetFileName(argv[2]);
  solution->Update();

  std::array<double, 6> bounds;
  solution->GetOutput()->GetBounds(bounds.data());

  // Create an outline
  vtkNew<vtkGenericOutlineFilter> outline;
  outline->SetInputConnection(solution->GetOutputPort());

  // Create Seeds
  vtkNew<vtkPointSource> seeds;
  seeds->SetRadius(0.2);
  seeds->SetCenter(3.0, 1.6, 1.25);
  seeds->SetNumberOfPoints(50);

  // Create streamlines
  vtkNew<vtkStreamTracer> streamTracer;
  streamTracer->SetIntegrationDirectionToBoth();
  streamTracer->SetInputConnection(solution->GetOutputPort());
  streamTracer->SetSourceConnection(seeds->GetOutputPort());
  streamTracer->SetMaximumPropagation(50);
  streamTracer->SetInitialIntegrationStep(.2);
  streamTracer->SetMinimumIntegrationStep(.01);
  streamTracer->SetIntegratorType(2);
  streamTracer->SetComputeVorticity(1);

  vtkNew<vtkTubeFilter> tubes;
  tubes->SetInputConnection(streamTracer->GetOutputPort());
  tubes->SetNumberOfSides(8);
  tubes->SetRadius(.02);
  tubes->SetVaryRadius(0);

  vtkNew<vtkPolyDataMapper> mapTubes;
  mapTubes->SetInputConnection(tubes->GetOutputPort());
  mapTubes->SetScalarRange(solution->GetOutput()->GetScalarRange());

  vtkNew<vtkActor> tubesActor;
  tubesActor->SetMapper(mapTubes);

  // Create an Isosurface
  vtkNew<vtkContourFilter> isoSurface;
  isoSurface->SetValue(0, 500.0);
  isoSurface->SetInputConnection(solution->GetOutputPort());

  vtkNew<vtkPolyDataMapper> isoSurfaceMapper;
  isoSurfaceMapper->SetInputConnection(isoSurface->GetOutputPort());
  isoSurfaceMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> isoSurfaceActor;
  isoSurfaceActor->SetMapper(isoSurfaceMapper);
  isoSurfaceActor->GetProperty()->SetOpacity(.5);
  isoSurfaceActor->GetProperty()->SetDiffuseColor(isoSurfaceColor.GetData());

  vtkNew<vtkSphereSource> sphere;
  sphere->SetCenter(seeds->GetCenter());
  sphere->SetRadius(seeds->GetRadius());
  sphere->SetThetaResolution(20);
  sphere->SetPhiResolution(11);
  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphere->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetOpacity(1.0);
  sphereActor->GetProperty()->SetSpecular(.4);
  sphereActor->GetProperty()->SetSpecularPower(80);
  sphereActor->GetProperty()->SetDiffuseColor(sphereColor.GetData());

  renderer->AddActor(tubesActor);
  renderer->AddActor(sphereActor);
  renderer->AddActor(isoSurfaceActor);

  renderer->SetBackground(backgroundColor.GetData());
  renderWindow->SetSize(640, 512);
  renderWindow->SetWindowName("FireFlowDemo");
  renderWindow->Render();

  renderer->GetActiveCamera()->Azimuth(20.0);
  renderer->GetActiveCamera()->Elevation(10.0);
  renderer->GetActiveCamera()->Dolly(1.25);
  renderer->ResetCameraClippingRange();

  // Create widgets to manipulate point source center
  vtkNew<vtkSliderWidget> xWidget;
  MakeXWidget(xWidget, seeds, sphere, renderer, renderWindowInteractor);
  dynamic_cast<vtkSliderRepresentation2D*>(xWidget->GetRepresentation())
      ->SetMinimumValue(bounds[0]);
  dynamic_cast<vtkSliderRepresentation2D*>(xWidget->GetRepresentation())
      ->SetMaximumValue(bounds[1]);
  vtkNew<vtkSliderWidget> yWidget;
  MakeYWidget(yWidget, seeds, sphere, renderer, renderWindowInteractor);
  dynamic_cast<vtkSliderRepresentation2D*>(yWidget->GetRepresentation())
      ->SetMinimumValue(bounds[2]);
  dynamic_cast<vtkSliderRepresentation2D*>(yWidget->GetRepresentation())
      ->SetMaximumValue(bounds[3]);
  vtkNew<vtkSliderWidget> zWidget;
  MakeZWidget(zWidget, seeds, sphere, renderer, renderWindowInteractor);
  dynamic_cast<vtkSliderRepresentation2D*>(zWidget->GetRepresentation())
      ->SetMinimumValue(bounds[4]);
  dynamic_cast<vtkSliderRepresentation2D*>(zWidget->GetRepresentation())
      ->SetMaximumValue(bounds[5]);

  renderWindow->Render();
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
#define DELAY 500
// These callbacks do the actual work.
// Callbacks for the interactions
class SliderCallbackX : public vtkCommand
{
public:
  static SliderCallbackX* New()
  {
    return new SliderCallbackX;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    double center[3];
    this->PointSource->GetCenter(center);
    center[0] = value;
    this->PointSource->SetCenter(center);
    SphereSource->SetCenter(center);
    PointSource->Modified();
    PointSource->Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
  }
  SliderCallbackX() = default;
  vtkPointSource* PointSource = nullptr;
  vtkSphereSource* SphereSource = nullptr;
};

void MakeXWidget(vtkSliderWidget* widget, vtkPointSource* pointSource,
                 vtkSphereSource* sphereSource, vtkRenderer* renderer,
                 vtkRenderWindowInteractor* interactor)
{
  // Setup a slider widget for each varying parameter
  double tubeWidth(.005);
  double sliderLength(.02);
  double titleHeight(.02);
  double labelHeight(.02);

  vtkNew<vtkSliderRepresentation2D> sliderRepX;

  sliderRepX->SetRenderer(renderer);

  sliderRepX->SetMinimumValue(-1000.0);
  sliderRepX->SetMaximumValue(1000);
  sliderRepX->SetValue(pointSource->GetCenter()[0]);
  sliderRepX->SetTitleText("X");
  sliderRepX->SetRenderer(renderer);
  sliderRepX->GetPoint1Coordinate()->SetValue(0.1, 0.1);
  sliderRepX->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
  sliderRepX->GetPoint2Coordinate()->SetValue(0.3, 0.1);
  sliderRepX->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedViewport();

  sliderRepX->SetTubeWidth(tubeWidth);
  sliderRepX->SetSliderLength(sliderLength);
  sliderRepX->SetTitleHeight(titleHeight);
  sliderRepX->SetLabelHeight(labelHeight);
  sliderRepX->SetEndCapLength(tubeWidth * 1.5);
  sliderRepX->SetSliderWidth(tubeWidth * 1.5);
  sliderRepX->GetSliderProperty()->SetColor(0.0, 0.0, 0.0);
  sliderRepX->BuildRepresentation();

  widget->SetRepresentation(sliderRepX);
  widget->SetAnimationModeToAnimate();
  widget->SetNumberOfAnimationSteps(10);
  widget->SetInteractor(interactor);
  widget->EnabledOn();

  auto callbackX = vtkSmartPointer<SliderCallbackX>::New();
  callbackX->PointSource = pointSource;
  callbackX->SphereSource = sphereSource;
  widget->AddObserver(vtkCommand::InteractionEvent, callbackX);
}

class SliderCallbackY : public vtkCommand
{
public:
  static SliderCallbackY* New()
  {
    return new SliderCallbackY;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    double center[3];
    this->PointSource->GetCenter(center);
    center[1] = value;
    this->PointSource->SetCenter(center);
    this->SphereSource->SetCenter(center);
    PointSource->Modified();
    PointSource->Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
  }
  SliderCallbackY() = default;
  vtkPointSource* PointSource = nullptr;
  vtkSphereSource* SphereSource = nullptr;
};

void MakeYWidget(vtkSliderWidget* widget, vtkPointSource* pointSource,
                 vtkSphereSource* sphereSource, vtkRenderer* renderer,
                 vtkRenderWindowInteractor* interactor)
{
  // Setup a slider widget for each varying parameter
  double tubeWidth(.005);
  double sliderLength(.02);
  double titleHeight(.02);
  double labelHeight(.02);

  vtkNew<vtkSliderRepresentation2D> sliderRepY;

  sliderRepY->SetRenderer(renderer);

  sliderRepY->SetMinimumValue(-1000.0);
  sliderRepY->SetMaximumValue(1000);
  sliderRepY->SetValue(pointSource->GetCenter()[1]);
  sliderRepY->SetTitleText("Y");
  sliderRepY->SetRenderer(renderer);
  sliderRepY->GetPoint1Coordinate()->SetValue(0.4, 0.1);
  sliderRepY->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
  sliderRepY->GetPoint2Coordinate()->SetValue(0.6, 0.1);
  sliderRepY->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedViewport();

  sliderRepY->SetTubeWidth(tubeWidth);
  sliderRepY->SetSliderLength(sliderLength);
  sliderRepY->SetTitleHeight(titleHeight);
  sliderRepY->SetLabelHeight(labelHeight);
  sliderRepY->SetEndCapLength(tubeWidth * 1.5);
  sliderRepY->SetSliderWidth(tubeWidth * 1.5);
  sliderRepY->GetSliderProperty()->SetColor(0.0, 0.0, 0.0);
  sliderRepY->BuildRepresentation();

  widget->SetRepresentation(sliderRepY);
  widget->SetAnimationModeToAnimate();
  widget->SetNumberOfAnimationSteps(10);
  widget->SetInteractor(interactor);
  widget->EnabledOn();

  auto callbackY = vtkSmartPointer<SliderCallbackY>::New();
  callbackY->PointSource = pointSource;
  callbackY->SphereSource = sphereSource;

  widget->AddObserver(vtkCommand::InteractionEvent, callbackY);
}

class SliderCallbackZ : public vtkCommand
{
public:
  static SliderCallbackZ* New()
  {
    return new SliderCallbackZ;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    double center[3];
    this->PointSource->GetCenter(center);
    center[2] = value;
    this->PointSource->SetCenter(center);
    this->SphereSource->SetCenter(center);
    PointSource->Modified();
    PointSource->Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
  }
  SliderCallbackZ() = default;
  vtkPointSource* PointSource = nullptr;
  vtkSphereSource* SphereSource = nullptr;
};

void MakeZWidget(vtkSliderWidget* widget, vtkPointSource* pointSource,
                 vtkSphereSource* sphereSource, vtkRenderer* renderer,
                 vtkRenderWindowInteractor* interactor)
{
  // Setup a slider widget for each varying parameter
  double tubeWidth(.005);
  double sliderLength(.02);
  double titleHeight(.02);
  double labelHeight(.02);

  vtkNew<vtkSliderRepresentation2D> sliderRepZ;

  sliderRepZ->SetRenderer(renderer);

  sliderRepZ->SetMinimumValue(-1000.0);
  sliderRepZ->SetMaximumValue(1000);
  sliderRepZ->SetValue(pointSource->GetCenter()[2]);
  sliderRepZ->SetTitleText("Z");
  sliderRepZ->SetRenderer(renderer);
  sliderRepZ->GetPoint1Coordinate()->SetValue(0.7, 0.1);
  sliderRepZ->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
  sliderRepZ->GetPoint2Coordinate()->SetValue(0.9, 0.1);
  sliderRepZ->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedViewport();

  sliderRepZ->SetTubeWidth(tubeWidth);
  sliderRepZ->SetSliderLength(sliderLength);
  sliderRepZ->SetTitleHeight(titleHeight);
  sliderRepZ->SetLabelHeight(labelHeight);
  sliderRepZ->SetEndCapLength(tubeWidth * 1.5);
  sliderRepZ->SetSliderWidth(tubeWidth * 1.5);
  sliderRepZ->GetSliderProperty()->SetColor(0.0, 0.0, 0.0);
  sliderRepZ->BuildRepresentation();

  widget->SetRepresentation(sliderRepZ);
  widget->SetAnimationModeToAnimate();
  widget->SetNumberOfAnimationSteps(10);
  widget->SetInteractor(interactor);
  widget->EnabledOn();

  auto callbackZ = vtkSmartPointer<SliderCallbackZ>::New();
  callbackZ->PointSource = pointSource;
  callbackZ->SphereSource = sphereSource;

  widget->AddObserver(vtkCommand::InteractionEvent, callbackZ);
}
} // namespace
