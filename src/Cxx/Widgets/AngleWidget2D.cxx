#include <vtkActor.h>
#include <vtkAngleRepresentation2D.h>
#include <vtkAngleWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int /* argc */, char* /* argv */[])
{
  vtkNew<vtkNamedColors> colors;

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("AngleWidget2D");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  double pos1[3] = {50.0, 200.0, 0.0};
  double pos2[3] = {200.0, 20.0, 0.0};
  double center[3] = {100.0, 100.0, 0.0};
  vtkNew<vtkAngleRepresentation2D> rep;
  rep->ArcVisibilityOff();

  vtkNew<vtkAngleWidget> angleWidget;
  angleWidget->CreateDefaultRepresentation();
  angleWidget->SetRepresentation(rep);
  angleWidget->SetInteractor(renderWindowInteractor);

  renderWindow->Render();

  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  renderWindowInteractor->Initialize();
  renderWindow->Render();
  angleWidget->On();
  rep->SetPoint1DisplayPosition(pos1);
  rep->SetPoint2DisplayPosition(pos2);
  rep->SetCenterDisplayPosition(center);
  rep->Ray1VisibilityOn();
  rep->Ray2VisibilityOn();
  rep->ArcVisibilityOn();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
