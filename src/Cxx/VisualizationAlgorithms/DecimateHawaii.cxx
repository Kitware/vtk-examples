#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDecimatePro.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " honolulu.vtk" << std::endl;
    return EXIT_FAILURE;
  }

  // This example shows how to use decimation to reduce a polygonal mesh. We
  // also use mesh smoothing and generate surface normals to give a pleasing
  // result.
  //

  vtkNew<vtkPolyDataReader> hawaii;
  hawaii->SetFileName(argv[1]);

  // We want to preserve topology (not let any cracks form). This may limit
  // the total reduction possible, which we have specified at 90%.
  //
  vtkNew<vtkDecimatePro> deci;
  deci->SetInputConnection(hawaii->GetOutputPort());
  deci->SetTargetReduction(0.9);
  deci->PreserveTopologyOn();

  vtkNew<vtkPolyDataNormals> decimatedNormals;
  decimatedNormals->SetInputConnection(deci->GetOutputPort());
  decimatedNormals->FlipNormalsOn();
  decimatedNormals->SetFeatureAngle(60);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> decimatedMapper;
  decimatedMapper->SetInputConnection(decimatedNormals->GetOutputPort());

  vtkNew<vtkActor> decimatedActor;
  decimatedActor->SetMapper(decimatedMapper);
  decimatedActor->GetProperty()->SetColor(colors->GetColor3d("Sienna").GetData());
  decimatedActor->GetProperty()->SetRepresentationToWireframe();

  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputConnection(decimatedNormals->GetOutputPort());

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetColor(colors->GetColor3d("Sienna").GetData());

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkRenderer> renderer1;
  renderer1->SetViewport(0.0, 0.0, 0.5, 1.0);

  vtkNew<vtkRenderer> renderer2;
  renderer2->SetViewport(0.5, 0.0, 1.0, 1.0);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer1);
  renderWindow->AddRenderer(renderer2);
  renderWindow->SetWindowName("DecimateHawaii");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer, set the background and size
  //
  renderer1->AddActor(originalActor);
  renderer2->AddActor(decimatedActor);
  renderer1->SetBackground(colors->GetColor3d("Wheat").GetData());
  renderer2->SetBackground(colors->GetColor3d("Papaya_Whip").GetData());
  renderWindow->SetSize(800, 400);

  // render the image
  //
  vtkNew<vtkCamera> cam1;
  renderer1->SetActiveCamera(cam1);
  renderer2->SetActiveCamera(cam1);
  renderer1->ResetCamera();
  cam1->Elevation(-30);
  cam1->Dolly(1.2);
  renderer1->ResetCameraClippingRange();
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
