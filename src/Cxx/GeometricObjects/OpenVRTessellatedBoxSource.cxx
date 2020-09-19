#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderer.h>
#include <vtkOpenVRRenderWindowInteractor.h>
#include <vtkShrinkFilter.h>
#include <vtkTessellatedBoxSource.h>

int main(int /* argc */, char * /* argv */[])
{
  double bounds[6];
  bounds[0] = -10.0; bounds[1] = 10.0;
  bounds[2] = 10.0; bounds[3] = 20.0;
  bounds[4] = -5.0; bounds[5] = 5.0;

  vtkNew<vtkTessellatedBoxSource> boxSource;
  boxSource->SetLevel(3);
  boxSource->QuadsOn();
  boxSource->SetBounds(bounds);
  boxSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);

  vtkNew<vtkShrinkFilter> shrink;
  shrink->SetInputConnection(boxSource->GetOutputPort());
  shrink->SetShrinkFactor(.8);

  //Create a mapper and actor
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(shrink->GetOutputPort());

  vtkNew<vtkProperty> back;
  back->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->EdgeVisibilityOn();
  actor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
  actor->SetBackfaceProperty(back);

  //Create a renderer, render window, and interactor
  vtkNew<vtkOpenVRRenderer> renderer;
  vtkNew<vtkOpenVRRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("OpenVRTessellatedBoxSource");
  vtkNew<vtkOpenVRRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  //Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCameraClippingRange();

  //Render and interact
  renderWindow->SetSize(640, 480);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
