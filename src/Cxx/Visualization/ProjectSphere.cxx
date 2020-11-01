#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkElevationFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSuperEllipsoid.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProjectSphereFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkParametricSuperEllipsoid> surface;
  surface->SetN1(2.0);
  surface->SetN2(0.5);

  vtkNew<vtkParametricFunctionSource> source;
  source->SetParametricFunction(surface);

  vtkNew<vtkElevationFilter> elevationFilter;
  elevationFilter->SetInputConnection(source->GetOutputPort());
  elevationFilter->SetLowPoint(0.0, 0.0, -4.0);
  elevationFilter->SetHighPoint(0.0, 0.0, 4.0);
  elevationFilter->Update();

  // Deep copy the point data since in some versions of VTK,
  // the ProjectSphereFilter modifies the input point data
  vtkNew<vtkPolyData> pd1;
  pd1->DeepCopy(elevationFilter->GetOutput());

  vtkNew<vtkProjectSphereFilter> sphereProject1;
  sphereProject1->SetInputConnection(elevationFilter->GetOutputPort());
  sphereProject1->Update();

  vtkNew<vtkPolyDataMapper> mapper1;
  mapper1->SetInputConnection(sphereProject1->GetOutputPort());
  mapper1->SetScalarRange(
      sphereProject1->GetOutput()->GetPointData()->GetScalars()->GetRange());
  vtkNew<vtkActor> actor1;
  actor1->SetMapper(mapper1);

  vtkNew<vtkPolyDataMapper> mapper2;
  mapper2->SetInputData(pd1);
  mapper2->SetScalarRange(pd1->GetPointData()->GetScalars()->GetRange());
  vtkNew<vtkActor> actor2;
  actor2->SetMapper(mapper2);

  // A render window
  vtkNew<vtkRenderWindow> renderWindow;

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("RosyBrown").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("CadetBlue").GetData());

  leftRenderer->AddActor(actor2);
  rightRenderer->AddActor(actor1);

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  leftRenderer->GetActiveCamera()->Azimuth(30);
  leftRenderer->GetActiveCamera()->Elevation(-30);
  leftRenderer->ResetCamera();

  // Render an image (lights and cameras are created automatically)
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("ProjectSphere");

  renderWindow->Render();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
