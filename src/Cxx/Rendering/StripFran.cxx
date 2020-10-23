#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDecimatePro.h>
#include <vtkMaskPolyData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStripper.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " fran_cut.vtk" << std::endl;
    return EXIT_FAILURE;
  }
  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer1;
  renderer1->SetViewport(0., 0., 0.5, 1.);

  vtkNew<vtkRenderer> renderer2;
  renderer2->SetViewport(0.5, 0., 1., 1.);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer1);
  renderWindow->AddRenderer(renderer2);
  renderWindow->SetWindowName("StripFran");


  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // create a cyberware source
  //
  vtkNew<vtkPolyDataReader> cyber;
  cyber->SetFileName(argv[1]);

  vtkNew<vtkDecimatePro> deci;
  deci->SetInputConnection(cyber->GetOutputPort());
  deci->SetTargetReduction(0.7);
  deci->PreserveTopologyOn();

  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputConnection(deci->GetOutputPort());

  vtkNew<vtkMaskPolyData> mask;
  mask->SetInputConnection(deci->GetOutputPort());
  mask->SetOnRatio(2);

  vtkNew<vtkPolyDataMapper> cyberMapper;
  cyberMapper->SetInputConnection(mask->GetOutputPort());

  vtkNew<vtkActor> cyberActor;
  cyberActor->SetMapper(cyberMapper);
  cyberActor->GetProperty()->SetColor(colors->GetColor3d("Flesh").GetData());

  vtkNew<vtkStripper> stripper;
  stripper->SetInputConnection(cyber->GetOutputPort());

  vtkNew<vtkMaskPolyData> stripperMask;
  stripperMask->SetInputConnection(stripper->GetOutputPort());
  stripperMask->SetOnRatio(2);

  vtkNew<vtkPolyDataMapper> stripperMapper;
  stripperMapper->SetInputConnection(stripperMask->GetOutputPort());

  vtkNew<vtkActor> stripperActor;
  stripperActor->SetMapper(stripperMapper);
  stripperActor->GetProperty()->SetColor(colors->GetColor3d("Flesh").GetData());

  // Add the actors to the renderer, set the background and size
  //
  renderer1->AddActor(stripperActor);
  renderer2->AddActor(cyberActor);
  renderer1->SetBackground(colors->GetColor3d("Wheat").GetData());
  renderer2->SetBackground(colors->GetColor3d("Papaya_Whip").GetData());
  renderWindow->SetSize(1024, 640);

  // render the image
  //
  vtkNew<vtkCamera> cam1;
  cam1->SetFocalPoint(0, 0, 0);
  cam1->SetPosition(1, 0, 0);
  cam1->SetViewUp(0, 1, 0);
  renderer1->SetActiveCamera(cam1);
  renderer2->SetActiveCamera(cam1);
  renderer1->ResetCamera();
  cam1->Azimuth(30);
  cam1->Elevation(30);
  cam1->Dolly(1.4);
  renderer1->ResetCameraClippingRange();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
