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
#include <vtkTexture.h>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " fran_cut.vtk fran_cut.png"
              << std::endl;
    return EXIT_FAILURE;
  }

  // This example shows how to use decimation to reduce a polygonal mesh. We
  // also use mesh smoothing and generate surface normals to give a pleasing
  // result.
  //

  // We start by reading some data that was originally captured from
  // a Cyberware laser digitizing system.
  //
  vtkNew<vtkPolyDataReader> fran;
  fran->SetFileName(argv[1]);

  // Read the corresponding texture
  vtkNew<vtkPNGReader> textureReader;
  textureReader->SetFileName(argv[2]);

  vtkNew<vtkTexture> texture;
  texture->InterpolateOn();
  texture->SetInputConnection(textureReader->GetOutputPort());

  // We want to preserve topology (not let any cracks form). This may limit
  // the total reduction possible, which we have specified at 90%.
  //
  vtkNew<vtkDecimatePro> deci;
  deci->SetInputConnection(fran->GetOutputPort());
  deci->SetTargetReduction(0.9);
  deci->PreserveTopologyOn();

  vtkNew<vtkPolyDataNormals> decimatedNormals;
  decimatedNormals->SetInputConnection(deci->GetOutputPort());
  decimatedNormals->FlipNormalsOn();
  decimatedNormals->SetFeatureAngle(60);

  vtkNew<vtkPolyDataNormals> originalNormals;
  originalNormals->SetInputConnection(fran->GetOutputPort());
  originalNormals->FlipNormalsOn();
  originalNormals->SetFeatureAngle(60);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> decimatedMapper;
  decimatedMapper->SetInputConnection(decimatedNormals->GetOutputPort());

  vtkNew<vtkActor> decimatedActor;
  decimatedActor->SetMapper(decimatedMapper);
  decimatedActor->GetProperty()->SetAmbient(.5);
  decimatedActor->GetProperty()->SetDiffuse(.5);
  decimatedActor->SetTexture(texture);

  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputConnection(originalNormals->GetOutputPort());

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetAmbient(.5);
  originalActor->GetProperty()->SetDiffuse(.5);
  originalActor->SetTexture(texture);

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkRenderer> renderer1;
  renderer1->SetViewport(0.0, 0.0, 0.5, 1.0);

  vtkNew<vtkRenderer> renderer2;
  renderer2->SetViewport(0.5, 0.0, 1.0, 1.0);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer1);
  renderWindow->AddRenderer(renderer2);
  renderWindow->SetWindowName("DecimateFran");

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
  cam1->SetClippingRange(0.0475572, 2.37786);
  cam1->SetFocalPoint(0.052665, -0.129454, -0.0573973);
  cam1->SetPosition(0.327637, -0.116299, -0.256418);
  cam1->SetViewUp(-0.0225386, 0.999137, 0.034901);
  renderer1->SetActiveCamera(cam1);
  renderer2->SetActiveCamera(cam1);

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
