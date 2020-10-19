#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPointLocator.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {

class KeyPressInteractorStyle3 : public vtkInteractorStyleTrackballCamera
{
public:
  static KeyPressInteractorStyle3* New();

  KeyPressInteractorStyle3() : PointsPerBucket(1)
  {
    pointLocator = nullptr;
    renderWindow = nullptr;
    polydata = nullptr;
  }

  virtual void OnChar()
  {
    vtkRenderWindowInteractor* rwi = this->Interactor;
    char ch = rwi->GetKeyCode();

    switch (ch)
    {
    case 'n':
      cout << "Next." << endl;
      this->PointsPerBucket++;
      break;
    case 'p':
      cout << "Previous." << endl;
      if (this->PointsPerBucket > 1)
      {
        this->PointsPerBucket--;
      }
      break;
    default:
      std::cout << "An unhandled key was pressed." << std::endl;
      break;
    }

    std::cout << "PointsPerBucket = " << this->PointsPerBucket << std::endl;

    // Create the tree
    pointLocator->SetNumberOfPointsPerBucket(this->PointsPerBucket);
    pointLocator->BuildLocator();
    pointLocator->GenerateRepresentation(1, polydata);

    renderWindow->Render();

    // Forward events
    vtkInteractorStyleTrackballCamera::OnChar();
  }

  vtkPointLocator* pointLocator;
  vtkRenderWindow* renderWindow;
  vtkPolyData* polydata;

private:
  unsigned int PointsPerBucket;
};

vtkStandardNewMacro(KeyPressInteractorStyle3);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(4000);
  pointSource->Update();

  // Create the tree
  vtkNew<vtkPointLocator> pointLocator;
  pointLocator->SetDataSet(pointSource->GetOutput());
  pointLocator->AutomaticOn();
  pointLocator->SetNumberOfPointsPerBucket(2);
  pointLocator->BuildLocator();

  std::cout << "There are " << pointLocator->GetLevel() << " levels."
            << std::endl;
  std::cout << "There are " << pointLocator->GetNumberOfPointsPerBucket()
            << " points per bucket." << endl;

  vtkNew<vtkPolyData> polydata;
  pointLocator->GenerateRepresentation(pointLocator->GetLevel(), polydata);

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polydata);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Peru").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("PointLocator");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<KeyPressInteractorStyle3> style;
  style->pointLocator = pointLocator;
  style->renderWindow = renderWindow;
  style->polydata = polydata;

  renderWindowInteractor->SetInteractorStyle(style);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Beige").GetData());
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Elevation(60.0);
  renderer->GetActiveCamera()->Azimuth(30.0);
  renderWindow->SetWindowName("PointLocator");
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
