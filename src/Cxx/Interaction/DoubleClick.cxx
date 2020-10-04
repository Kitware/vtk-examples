#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {
// Define interaction style
class MouseInteractorStyleDoubleClick : public vtkInteractorStyleTrackballCamera
{
public:
  static MouseInteractorStyleDoubleClick* New();
  vtkTypeMacro(MouseInteractorStyleDoubleClick,
               vtkInteractorStyleTrackballCamera);

  MouseInteractorStyleDoubleClick() : NumberOfClicks(0), ResetPixelDistance(5)
  {
    this->PreviousPosition[0] = 0;
    this->PreviousPosition[1] = 0;
  }

  virtual void OnLeftButtonDown() override
  {
    this->NumberOfClicks++;
    int pickPosition[2];
    this->GetInteractor()->GetEventPosition(pickPosition);

    int xdist = pickPosition[0] - this->PreviousPosition[0];
    int ydist = pickPosition[1] - this->PreviousPosition[1];

    this->PreviousPosition[0] = pickPosition[0];
    this->PreviousPosition[1] = pickPosition[1];

    int moveDistance = (int)sqrt((double)(xdist * xdist + ydist * ydist));

    // Reset numClicks - If mouse moved further than resetPixelDistance
    if (moveDistance > this->ResetPixelDistance)
    {
      this->NumberOfClicks = 1;
    }

    if (this->NumberOfClicks == 2)
    {
      std::cout << "Double clicked." << std::endl;
      this->NumberOfClicks = 0;
    }

    // forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  }

private:
  unsigned int NumberOfClicks;
  int PreviousPosition[2];
  int ResetPixelDistance;
};
vtkStandardNewMacro(MouseInteractorStyleDoubleClick);
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5.0);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Silver").GetData());

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->AddActor(actor);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<MouseInteractorStyleDoubleClick> style;
  renderWindowInteractor->SetInteractorStyle(style);

  renderWindow->SetWindowName("DoubleClick");
  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
