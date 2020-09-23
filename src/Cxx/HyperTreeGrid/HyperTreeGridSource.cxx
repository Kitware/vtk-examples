#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkHyperTreeGridSource.h>
#include <vtkHyperTreeGridToUnstructuredGrid.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShrinkFilter.h>
#include <vtkVersion.h>

#include <cstdlib>

int main(int, char*[])
{
  // Create hyper tree grid source
  vtkNew<vtkHyperTreeGridSource> source;
#if VTK_VERSION_NUMBER >= 89000000000ULL
  source->SetMaxDepth(6);
#else
  source->SetMaximumLevel(6);
#endif
  source->SetDimensions(4, 4, 3); // GridCell 3, 3, 2
  source->SetGridScale(1.5, 1.0, 0.7);
  source->SetBranchFactor(4);
  source->SetDescriptor(
      "RRR .R. .RR ..R ..R .R.|R.......................... "
      "........................... ........................... "
      ".............R............. ....RR.RR........R......... "
      ".....RRRR.....R.RR......... ........................... "
      "........................... "
      "...........................|........................... "
      "........................... ........................... "
      "...RR.RR.......RR.......... ........................... "
      "RR......................... ........................... "
      "........................... ........................... "
      "........................... ........................... "
      "........................... ........................... "
      "............RRR............|........................... "
      "........................... .......RR.................. "
      "........................... ........................... "
      "........................... ........................... "
      "........................... ........................... "
      "........................... "
      "...........................|........................... "
      "...........................");
  source->Update();

  // Hyper tree grid to unstructured grid filter
  vtkNew<vtkHyperTreeGridToUnstructuredGrid> htg2ug;
  htg2ug->SetInputConnection(source->GetOutputPort());
  htg2ug->Update();

  vtkNew<vtkShrinkFilter> shrink;
  shrink->SetInputConnection(htg2ug->GetOutputPort());
  shrink->SetShrinkFactor(0.8);

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(shrink->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Burlywood").GetData());

  // Create the RenderWindow, Renderer and Interactor
  //
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->AddActor(actor);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(150);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCameraClippingRange();

  renderWindow->SetSize(640, 480);
  renderWindow->Render();
  renderWindow->SetWindowName("HyperTreeGridSource");
  interactor->Start();

  return EXIT_SUCCESS;
}
