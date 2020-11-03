#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkGenericOutlineFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkStreamTracer.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVRMLImporter.h>
#include <vtkXMLUnstructuredGridReader.h>

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

  // Create an outline
  vtkNew<vtkGenericOutlineFilter> outline;
  outline->SetInputConnection(solution->GetOutputPort());

  // Create Seeds
  vtkNew<vtkPointSource> seeds;
  seeds->SetRadius(0.2);
  seeds->SetCenter(3.5, 0.625, 1.25);
  seeds->SetNumberOfPoints(50);

  // Create streamlines
  vtkNew<vtkStreamTracer> streamTracer;
  streamTracer->SetIntegrationDirectionToBoth();
  streamTracer->SetInputConnection(solution->GetOutputPort());
  streamTracer->SetSourceConnection(seeds->GetOutputPort());
  streamTracer->SetMaximumPropagation(50);
  streamTracer->SetInitialIntegrationStep(.2);
  streamTracer->SetMinimumIntegrationStep(.01);
  streamTracer->SetIntegratorType(1);
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
  isoSurface->SetValue(0, 550.0);
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
  ;
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
  renderWindow->SetWindowName("FireFlow");
  renderWindow->Render();

  renderer->GetActiveCamera()->Azimuth(20.0);
  renderer->GetActiveCamera()->Elevation(10.0);
  renderer->GetActiveCamera()->Dolly(1.25);
  renderer->ResetCameraClippingRange();

  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
