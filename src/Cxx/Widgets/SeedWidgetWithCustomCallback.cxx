#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSeedRepresentation.h>
#include <vtkSeedWidget.h>
#include <vtkSphereSource.h>

namespace {
class vtkSeedCallback : public vtkCommand
{
public:
  static vtkSeedCallback* New()
  {
    return new vtkSeedCallback;
  }

  vtkSeedCallback()
  {
  }

  virtual void Execute(vtkObject*, unsigned long event, void* calldata)
  {
    if (event == vtkCommand::PlacePointEvent)
    {
      std::cout << "Point placed, total of: "
                << this->SeedRepresentation->GetNumberOfSeeds() << std::endl;
    }
    if (event == vtkCommand::InteractionEvent)
    {
      if (calldata)
      {
        std::cout << "Interacting with seed : "
                  << *(static_cast<int*>(calldata)) << std::endl;
      }
    }

    std::cout << "List of seeds (Display coordinates):" << std::endl;
    for (vtkIdType i = 0; i < this->SeedRepresentation->GetNumberOfSeeds(); i++)
    {
      double pos[3];
      this->SeedRepresentation->GetSeedDisplayPosition(i, pos);
      std::cout << "(" << pos[0] << " " << pos[1] << " " << pos[2] << ")"
                << std::endl;
    }
  }

  void SetRepresentation(vtkSeedRepresentation* rep)
  {
    this->SeedRepresentation = rep;
  }

private:
  vtkSeedRepresentation* SeedRepresentation;
};
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MidnightBlue").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("SeedWidgetWithCustomCallback");

  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->AddActor(actor);

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Create the representation
  vtkNew<vtkPointHandleRepresentation2D> handle;
  handle->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
  vtkNew<vtkSeedRepresentation> rep;
  rep->SetHandleRepresentation(handle);

  // Seed widget
  vtkNew<vtkSeedWidget> seedWidget;
  seedWidget->SetInteractor(renderWindowInteractor);
  seedWidget->SetRepresentation(rep);

  vtkNew<vtkSeedCallback> seedCallback;
  seedCallback->SetRepresentation(rep);
  seedWidget->AddObserver(vtkCommand::PlacePointEvent, seedCallback);
  seedWidget->AddObserver(vtkCommand::InteractionEvent, seedCallback);

  renderWindow->Render();

  renderWindowInteractor->Initialize();
  renderWindow->Render();
  seedWidget->On();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
