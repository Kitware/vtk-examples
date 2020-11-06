#include <vtkActor.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // create a rendering window and renderer;
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);
  renWin->SetWindowName("OrientationMarkerWidget");

  // create a renderwindowinteractor;
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkCubeSource> cube;
  cube->SetXLength(200);
  cube->SetYLength(200);
  cube->SetZLength(200);
  cube->Update();
  vtkNew<vtkPolyDataMapper> cm;
  cm->SetInputConnection(cube->GetOutputPort());
  vtkNew<vtkActor> ca;
  ca->SetMapper(cm);
  ca->GetProperty()->SetColor(colors->GetColor3d("BurlyWood").GetData());
  ca->GetProperty()->EdgeVisibilityOn();
  ca->GetProperty()->SetEdgeColor(colors->GetColor3d("Red").GetData());

  // assign actor to the renderer;
  ren->AddActor(ca);
  ren->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkAnnotatedCubeActor> axesActor;
  axesActor->SetXPlusFaceText("L");
  axesActor->SetXMinusFaceText("R");
  axesActor->SetYMinusFaceText("I");
  axesActor->SetYPlusFaceText("S");
  axesActor->SetZMinusFaceText("P");
  axesActor->SetZPlusFaceText("A");
  axesActor->GetTextEdgesProperty()->SetColor(
      colors->GetColor3d("Yellow").GetData());
  axesActor->GetTextEdgesProperty()->SetLineWidth(2);
  axesActor->GetCubeProperty()->SetColor(colors->GetColor3d("Blue").GetData());
  vtkNew<vtkOrientationMarkerWidget> axes;
  axes->SetOrientationMarker(axesActor);
  axes->SetInteractor(iren);
  axes->EnabledOn();
  axes->InteractiveOn();
  ren->ResetCamera();

  // enable user interface interactor;
  iren->Initialize();
  renWin->Render();
  ren->GetActiveCamera()->Azimuth(45);
  ren->GetActiveCamera()->Elevation(30);
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
