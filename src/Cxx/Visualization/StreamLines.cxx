#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStreamTracer.h>
#include <vtkStructuredGridOutlineFilter.h>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Required arguments: xyzFile qFile e.g. combxyz.bin combq.bin"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> namedColors;

  std::string xyzFile = argv[1]; // "combxyz.bin";
  std::string qFile = argv[2];   // "combq.bin";

  vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
  pl3d->SetXYZFileName(xyzFile.c_str());
  pl3d->SetQFileName(qFile.c_str());
  pl3d->SetScalarFunctionNumber(100);
  pl3d->SetVectorFunctionNumber(202);
  pl3d->Update();

  // Source of the streamlines
  vtkNew<vtkPlaneSource> seeds;
  seeds->SetXResolution(4);
  seeds->SetYResolution(4);
  seeds->SetOrigin(2, -2, 26);
  seeds->SetPoint1(2, 2, 26);
  seeds->SetPoint2(2, -2, 32);

  // Streamline itself
  vtkNew<vtkStreamTracer> streamLine;
  pl3d->Update();
  streamLine->SetInputData(pl3d->GetOutput()->GetBlock(0));
  streamLine->SetSourceConnection(seeds->GetOutputPort());
  streamLine->SetMaximumPropagation(100);
  streamLine->SetInitialIntegrationStep(0.1);
  streamLine->SetIntegrationDirectionToBoth();

  // streamLine->SetStartPosition(2,-2,30);
  // as alternative to the SetSource(), which can handle multiple
  // streamlines, you can set a SINGLE streamline from
  // SetStartPosition()

  vtkNew<vtkPolyDataMapper> streamLineMapper;
  streamLineMapper->SetInputConnection(streamLine->GetOutputPort());

  vtkNew<vtkActor> streamLineActor;
  streamLineActor->SetMapper(streamLineMapper);
  streamLineActor->VisibilityOn();

  // Outline-Filter for the grid
  vtkNew<vtkStructuredGridOutlineFilter> outline;
  outline->SetInputData(pl3d->GetOutput()->GetBlock(0));

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(
      namedColors->GetColor3d("White").GetData());

  // Create the RenderWindow, Renderer and Actors
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("StreamLines");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  interactor->SetInteractorStyle(style);

  renderer->AddActor(streamLineActor);
  renderer->AddActor(outlineActor);

  // Add the actors to the renderer, set the background and size
  renderer->SetBackground(namedColors->GetColor3d("MidnightBlue").GetData());
  renderWindow->SetSize(300, 300);
  interactor->Initialize();
  renderWindow->Render();
  renderer->GetActiveCamera()->SetPosition(-32.8, -12.3, 46.3);
  renderer->GetActiveCamera()->SetFocalPoint(8.3, 0.03, 29.8);
  renderer->GetActiveCamera()->SetViewUp(0.2, 0.5, 0.9);
  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
}
