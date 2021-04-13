#include <vtkActor.h>
#include <vtkCylinder.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVersion.h>

// vtkFlyingEdges3D was introduced in VTK >= 8.2
#if VTK_MAJOR_VERSION >= 9 || (VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 2)
#define USE_FLYING_EDGES
#else
#undef USE_FLYING_EDGES
#endif

#ifdef USE_FLYING_EDGES
#include <vtkFlyingEdges3D.h>
#else
#include <vtkMarchingCubes.h>
#endif

int main(int argc, char* argv[])
{
  int resolution = 50;
  if (argc > 1)
  {
    resolution = atoi(argv[1]);
  }

  vtkNew<vtkNamedColors> colors;

  // Create a sampled sphere
  vtkNew<vtkSphere> implicitSphere;
  double radius = 1.0;
  implicitSphere->SetRadius(radius);

  vtkNew<vtkSampleFunction> sampledSphere;
  sampledSphere->SetSampleDimensions(resolution, resolution, resolution);
  double xMin = -radius * 2.0;
  double xMax = radius * 2.0;
  sampledSphere->SetModelBounds(xMin, xMax, xMin, xMax, xMin, xMax);
  sampledSphere->SetImplicitFunction(implicitSphere);

#ifdef USE_FLYING_EDGES
  vtkNew<vtkFlyingEdges3D> isoSphere;
#else
  vtkNew<vtkMarchingCubes> isoSphere;
#endif
  isoSphere->SetValue(0, 1.0);
  isoSphere->SetInputConnection(sampledSphere->GetOutputPort());

  // Create a sampled cylinder
  vtkNew<vtkCylinder> implicitCylinder;
  implicitCylinder->SetRadius(radius / 2.0);
  vtkNew<vtkSampleFunction> sampledCylinder;
  sampledCylinder->SetSampleDimensions(resolution, resolution, resolution);
  sampledCylinder->SetModelBounds(xMin, xMax, xMin, xMax, xMin, xMax);
  sampledCylinder->SetImplicitFunction(implicitCylinder);

  // Probe cylinder with the sphere isosurface
  vtkNew<vtkProbeFilter> probeCylinder;
  probeCylinder->SetInputConnection(0, isoSphere->GetOutputPort());
  probeCylinder->SetInputConnection(1, sampledCylinder->GetOutputPort());
  probeCylinder->Update();

  // Restore the original normals
  probeCylinder->GetOutput()->GetPointData()->SetNormals(
      isoSphere->GetOutput()->GetPointData()->GetNormals());

  std::cout << "Scalar range: "
            << probeCylinder->GetOutput()->GetScalarRange()[0] << ", "
            << probeCylinder->GetOutput()->GetScalarRange()[1] << std::endl;

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapSphere;
  mapSphere->SetInputConnection(probeCylinder->GetOutputPort());
  mapSphere->SetScalarRange(probeCylinder->GetOutput()->GetScalarRange());

  vtkNew<vtkActor> sphere;
  sphere->SetMapper(mapSphere);

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("IsosurfaceSampling");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(sphere);
  renderer->SetBackground(colors->GetColor3d("AliceBlue").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
