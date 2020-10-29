#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkDepthSortPolyData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int argc, char* argv[])
{
  if (argc != 5)
  {
    std::cerr << "Usage: " << argv[0] << " DepthSortFlag ThetaResolution "
              << " PhiResolution ScalarVisibilityFlag" << std::endl;
    std::cerr << "1 100 100 0" << std::endl;
    return EXIT_FAILURE;
  }

  bool doDepthSort = atoi(argv[1]) == 1;
  int theta = atoi(argv[2]);
  int phi = atoi(argv[3]);
  bool scalarVisibility = atoi(argv[4]) == 1;

  vtkNew<vtkNamedColors> colors;

  // Create a bunch of spheres that overlap and cannot be easily arranged
  // so that the blending works without sorting. They are appended into a
  // single vtkPolyData because the filter only sorts within a single
  // vtkPolyData input.
  vtkNew<vtkAppendPolyData> appendData;

  for (int i = 0; i < 5; i++)
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetThetaResolution(theta);
    sphereSource->SetPhiResolution(phi);
    sphereSource->SetRadius(
        0.5); // all sphere except the center one should have radius = 0.5
    switch (i)
    {
    case 0:
      sphereSource->SetRadius(1);
      sphereSource->SetCenter(0, 0, 0);
      break;
    case 1:
      sphereSource->SetCenter(1, 0, 0);
      break;
    case 2:
      sphereSource->SetCenter(-1, 0, 0);
      break;
    case 3:
      sphereSource->SetCenter(0, 1, 0);
      break;
    case 4:
      sphereSource->SetCenter(0, -1, 0);
      break;
    }
    sphereSource->Update();
    appendData->AddInputConnection(sphereSource->GetOutputPort());
  }

  // The dephSort object is set up to generate scalars representing
  // the sort depth.  A camera is assigned for the sorting. The camera
  // defines the sort vector (position and focal point).
  vtkNew<vtkCamera> camera;
  vtkNew<vtkDepthSortPolyData> depthSort;

  depthSort->SetInputConnection(appendData->GetOutputPort());
  depthSort->SetDirectionToBackToFront();
  depthSort->SetVector(1, 1, 1);
  depthSort->SetCamera(camera);
  depthSort->SortScalarsOn();
  depthSort->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  if (doDepthSort)
  {
    mapper->SetInputConnection(depthSort->GetOutputPort());
  }
  else
  {
    mapper->SetInputConnection(appendData->GetOutputPort());
  }

  mapper->SetScalarVisibility(scalarVisibility);
  if (scalarVisibility)
  {
    mapper->SetScalarRange(0, depthSort->GetOutput()->GetNumberOfCells());
  }

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetOpacity(0.5);
  actor->GetProperty()->SetColor(colors->GetColor3d("Crimson").GetData());
  actor->RotateX(-72); // put the object in a position where it is easy
  // to see different overlapping regions

  // If an Prop3D is supplied, then its transformation matrix is taken
  // into account during sorting.
  depthSort->SetProp3D(actor);

  // Create the RenderWindow, Renderer and both Actors
  vtkNew<vtkRenderer> renderer;
  renderer->SetActiveCamera(camera);
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("DepthSortPolyData");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer, set the background and size
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderWindow->SetSize(600, 400);

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(2.2); // so the object is larger

  renderWindowInteractor->Initialize();
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
