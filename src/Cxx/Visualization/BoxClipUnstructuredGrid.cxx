#include <vtkBoxClipDataSet.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>

#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <vtkColor.h>
#include <vtkNamedColors.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.vtk" << endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;
  vtkColor3d backgroundColor = colors->GetColor3d("Silver");

  // Read the data
  vtkNew<vtkUnstructuredGridReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  double bounds[6];
  reader->GetOutput()->GetBounds(bounds);

  double range[2];
  reader->GetOutput()->GetScalarRange(range);

  double minBoxPoint[3];
  double maxBoxPoint[3];
  minBoxPoint[0] = (bounds[1] - bounds[0]) / 2.0 + bounds[0];
  minBoxPoint[1] = (bounds[3] - bounds[2]) / 2.0 + bounds[2];
  minBoxPoint[2] = (bounds[5] - bounds[4]) / 2.0 + bounds[4];
  maxBoxPoint[0] = bounds[1];
  maxBoxPoint[1] = bounds[3];
  maxBoxPoint[2] = bounds[5];

  vtkNew<vtkBoxClipDataSet> boxClip;
  boxClip->SetInputConnection(reader->GetOutputPort());
  boxClip->GenerateClippedOutputOn();

  const double minusx[] = {-1.0, -0.5, 0.0};
  const double minusy[] = {0.0, -1.0, 0.0};
  const double minusz[] = {0.0, 0.0, -1.0};
  const double plusx[] = {1.0, 0.0, 0.0};
  const double plusy[] = {0.0, 1.0, 0.0};
  const double plusz[] = {0.0, 0.0, 1.0};
  boxClip->SetBoxClip(minusx, minBoxPoint, minusy, minBoxPoint, minusz,
                      minBoxPoint, plusx, maxBoxPoint, plusy, maxBoxPoint,
                      plusz, maxBoxPoint);

  // Define a lut
  vtkNew<vtkLookupTable> lut1;
  lut1->SetHueRange(.667, 0);

  vtkNew<vtkDataSetMapper> mapperIn;
  mapperIn->SetInputConnection(boxClip->GetOutputPort());
  mapperIn->SetScalarRange(reader->GetOutput()->GetScalarRange());
  mapperIn->SetLookupTable(lut1);
  mapperIn->SetColorModeToMapScalars();

  vtkNew<vtkActor> actorIn;
  actorIn->SetMapper(mapperIn);

  vtkNew<vtkDataSetMapper> mapperOut;
  mapperOut->SetInputConnection(boxClip->GetOutputPort(1));
  mapperOut->SetScalarRange(reader->GetOutput()->GetScalarRange());
  mapperOut->SetLookupTable(lut1);
  mapperOut->SetColorModeToMapScalars();

  vtkNew<vtkActor> actorOut;
  actorOut->SetMapper(mapperOut);
  actorOut->AddPosition(-0.5 * (maxBoxPoint[0] - minBoxPoint[0]),
                        -0.5 * (maxBoxPoint[1] - minBoxPoint[1]),
                        -0.5 * (maxBoxPoint[2] - minBoxPoint[2]));

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(backgroundColor.GetData());
  renderer->UseHiddenLineRemovalOn();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("BoxClipUnstructuredGrid");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actorIn);
  renderer->AddActor(actorOut);

  // Generate an interesting view
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(120);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.0);
  renderer->ResetCameraClippingRange();

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
