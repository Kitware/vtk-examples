/*
Modified from VTK/Examples/GUI/Python/StreamlinesWithLineWidget.py.
This program encompasses the functionality of
StreamlinesWithLineWidget.tcl and LineWidget.tcl.

        Produce streamlines in the combustor dataset.

This example demonstrates how to use the vtkLineWidget to seed and
manipulate streamlines. Two line widgets are created. The first is invoked
by pressing 'i', the second by pressing 'L' (capital). Both can exist
together.

If the fourth parameter is non-zero:
 1) The third parameter value is changed to 25.
 2) The camera position and first line widget are positioned differently.
 3) The streamlines are displayed running from the first line widget.
 4) The second line widget is still available.
This is used to test the code.

*/

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkLineWidget.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRibbonFilter.h>
#include <vtkRungeKutta4.h>
#include <vtkStreamTracer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridOutlineFilter.h>

namespace {

class EnableActorCallback : public vtkCommand
{
public:
  static EnableActorCallback* New()
  {
    return new EnableActorCallback;
  }
  void Execute(vtkObject* /*caller*/, unsigned long, void*) override
  {
    this->actor->VisibilityOn();
  }
  EnableActorCallback() : actor(nullptr)
  {
  }
  vtkActor* actor;
};

// The Line Widget callback.
class LWCallback : public vtkCommand
{
public:
  static LWCallback* New()
  {
    return new LWCallback;
  }
  void Execute(vtkObject* caller, unsigned long, void*) override
  {
    vtkLineWidget* lineWidget = reinterpret_cast<vtkLineWidget*>(caller);
    lineWidget->GetPolyData(this->polyData);
    this->renWin->Render();
  }
  LWCallback() : polyData(nullptr), renWin(nullptr)
  {
  }
  vtkPolyData* polyData;
  vtkRenderWindow* renWin;
};
} // namespace

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0]
              << " filename1 filename2 [numOfStreamLines] [illustration]"
              << std::endl;
    std::cout << "where: filename1 is combxyz.bin and filename2 is combq.bin."
              << std::endl;
    std::cout
        << "       numOfStreamLines is the number of streamlines, default 25"
        << std::endl;
    std::cout << "       illustration, if non-zero, reproduce Fig 7-39 of the "
                 "VTK Textbook."
              << std::endl;
    return EXIT_FAILURE;
  }
  auto numOfStreamLines = 25;
  auto illustration = false;
  std::string fileName1 = argv[1];
  std::string fileName2 = argv[2];
  if (argc > 3)
  {
    numOfStreamLines = atoi(argv[3]);
  }
  if (argc > 4)
  {
    illustration = atoi(argv[4]) != 0;
    if (illustration)
    {
      numOfStreamLines = 25;
    }
  }

  vtkNew<vtkNamedColors> colors;

  // Start by loading some data.
  vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
  pl3d->SetXYZFileName(fileName1.c_str());
  pl3d->SetQFileName(fileName2.c_str());
  pl3d->SetScalarFunctionNumber(100); // Density
  pl3d->SetVectorFunctionNumber(202); // Momentum
  pl3d->Update();

  vtkStructuredGrid* pl3d_output =
      dynamic_cast<vtkStructuredGrid*>(pl3d->GetOutput()->GetBlock(0));

  // Create the Renderer, RenderWindow and RenderWindowInteractor.
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Needed by: vtkStreamTracer and vtkLineWidget.
  vtkNew<vtkPolyData> seeds;
  vtkNew<vtkActor> streamline;
  vtkNew<vtkPolyData> seeds2;
  vtkNew<vtkActor> streamline2;

  // The line widget is used seed the streamlines.
  vtkNew<vtkLineWidget> lineWidget;
  lineWidget->SetResolution(numOfStreamLines);
  lineWidget->SetInputData(pl3d_output);
  lineWidget->GetPolyData(seeds);
  if (illustration)
  {
    lineWidget->SetAlignToNone();
    lineWidget->SetPoint1(0.974678, 5.073630, 31.217961);
    lineWidget->SetPoint2(0.457544, -4.995921, 31.080175);
  }
  else
  {
    lineWidget->SetAlignToYAxis();
  }
  lineWidget->ClampToBoundsOn();
  lineWidget->PlaceWidget();

  lineWidget->SetInteractor(iren);

  // Associate the line widget with the interactor and setup the callbacks.
  vtkNew<LWCallback> GenerateStreamlines;
  GenerateStreamlines->polyData = seeds;
  GenerateStreamlines->renWin = renWin;
  vtkNew<EnableActorCallback> BeginInteraction;
  BeginInteraction->actor = streamline;
  lineWidget->AddObserver(vtkCommand::StartInteractionEvent, BeginInteraction);
  lineWidget->AddObserver(vtkCommand::InteractionEvent, GenerateStreamlines);

  // The second line widget is used seed more streamlines.
  vtkNew<vtkLineWidget> lineWidget2;
  lineWidget2->SetResolution(numOfStreamLines);
  lineWidget2->SetInputData(pl3d_output);
  lineWidget2->GetPolyData(seeds2);
  lineWidget2->SetKeyPressActivationValue('L');
  lineWidget2->SetAlignToZAxis();
  lineWidget->ClampToBoundsOn();
  lineWidget2->PlaceWidget();

  lineWidget2->SetInteractor(iren);

  // Associate the line widget with the interactor and setup the callbacks.
  vtkNew<LWCallback> GenerateStreamlines2;
  GenerateStreamlines2->polyData = seeds2;
  GenerateStreamlines2->renWin = renWin;
  vtkNew<EnableActorCallback> BeginInteraction2;
  BeginInteraction2->actor = streamline2;
  lineWidget2->AddObserver(vtkCommand::StartInteractionEvent,
                           BeginInteraction2);
  lineWidget2->AddObserver(vtkCommand::InteractionEvent, GenerateStreamlines2);

  // Here we set up two streamlines.
  vtkNew<vtkRungeKutta4> rk4;
  vtkNew<vtkStreamTracer> streamer;
  streamer->SetInputData(pl3d_output);
  streamer->SetSourceData(seeds);
  streamer->SetMaximumPropagation(100);
  streamer->SetInitialIntegrationStep(0.2);
  streamer->SetIntegrationDirectionToForward();
  streamer->SetComputeVorticity(1);
  streamer->SetIntegrator(rk4);
  vtkNew<vtkRibbonFilter> rf;
  rf->SetInputConnection(streamer->GetOutputPort());
  rf->SetWidth(0.1);
  rf->SetWidthFactor(5);
  vtkNew<vtkPolyDataMapper> streamMapper;
  streamMapper->SetInputConnection(rf->GetOutputPort());
  streamMapper->SetScalarRange(pl3d_output->GetScalarRange());
  streamline->SetMapper(streamMapper);
  streamline->VisibilityOff();

  vtkNew<vtkStreamTracer> streamer2;
  streamer2->SetInputData(pl3d_output);
  streamer2->SetSourceData(seeds2);
  streamer2->SetMaximumPropagation(100);
  streamer2->SetInitialIntegrationStep(0.2);
  streamer2->SetIntegrationDirectionToForward();
  streamer2->SetComputeVorticity(1);
  streamer2->SetIntegrator(rk4);
  vtkNew<vtkRibbonFilter> rf2;
  rf2->SetInputConnection(streamer2->GetOutputPort());
  rf2->SetWidth(0.1);
  rf2->SetWidthFactor(5);
  vtkNew<vtkPolyDataMapper> streamMapper2;
  streamMapper2->SetInputConnection(rf2->GetOutputPort());
  streamMapper2->SetScalarRange(pl3d_output->GetScalarRange());
  streamline2->SetMapper(streamMapper2);
  streamline2->VisibilityOff();

  // Get an outline of the data set for context.
  vtkNew<vtkStructuredGridOutlineFilter> outline;
  outline->SetInputData(pl3d_output);
  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());
  vtkNew<vtkActor> outlineActor;
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());
  outlineActor->SetMapper(outlineMapper);

  // Add the actors to the renderer, set the background and size.
  ren->AddActor(outlineActor);
  ren->AddActor(streamline);
  ren->AddActor(streamline2);
  renWin->SetSize(512, 512);
  renWin->SetWindowName("StreamlinesWithLineWidget");

  vtkCamera* cam = ren->GetActiveCamera();
  if (illustration)
  {
    // We need to directly display the streamlines in this case.
    lineWidget->EnabledOn();
    streamline->VisibilityOn();
    lineWidget->GetPolyData(seeds);
    renWin->Render();

    cam->SetClippingRange(14.216207, 68.382915);
    cam->SetFocalPoint(9.718210, 0.458166, 29.399900);
    cam->SetPosition(-15.827551, -16.997463, 54.003120);
    cam->SetViewUp(0.616076, 0.179428, 0.766979);
    ren->SetBackground(colors->GetColor3d("Silver").GetData());
  }
  else
  {
    cam->SetClippingRange(3.95297, 50);
    cam->SetFocalPoint(9.71821, 0.458166, 29.3999);
    cam->SetPosition(2.7439, -37.3196, 38.7167);
    cam->SetViewUp(-0.16123, 0.264271, 0.950876);
    ren->SetBackground(colors->GetColor3d("Silver").GetData());
  }

  iren->Initialize();
  renWin->Render();
  iren->Start();
  return EXIT_SUCCESS;
}
