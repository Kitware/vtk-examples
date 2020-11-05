#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkInteractorStyleImage.h>
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
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>

#include <array>

namespace {
class vtkSeedImageCallback : public vtkCommand
{
public:
  static vtkSeedImageCallback* New()
  {
    return new vtkSeedImageCallback;
  }

  vtkSeedImageCallback() = default;

  virtual void Execute(vtkObject*, unsigned long event, void* calldata)
  {
    if (event == vtkCommand::PlacePointEvent)
    {
      std::cout << "Placing point..." << std::endl;
      std::cout << "There are now "
                << this->SeedRepresentation->GetNumberOfSeeds() << " seeds."
                << std::endl;
      for (unsigned int seedId = 0; static_cast<int>(seedId) <
           this->SeedRepresentation->GetNumberOfSeeds();
           seedId++)
      {
        double pos[3];
        this->SeedRepresentation->GetSeedDisplayPosition(seedId, pos);
        std::cout << "Seed " << seedId << " : (" << pos[0] << " " << pos[1]
                  << " " << pos[2] << ")" << std::endl;
      }
      return;
    }
    if (event == vtkCommand::InteractionEvent)
    {
      std::cout << "Interaction..." << std::endl;
      if (calldata)
      {
        double pos[3];
        this->SeedRepresentation->GetSeedDisplayPosition(0, pos);
        std::cout << "Moved to (" << pos[0] << " " << pos[1] << " " << pos[2]
                  << ")" << std::endl;
      }
      return;
    }
  }

  void SetRepresentation(vtkSmartPointer<vtkSeedRepresentation> rep)
  {
    this->SeedRepresentation = rep;
  }
  void SetWidget(vtkSmartPointer<vtkSeedWidget> widget)
  {
    this->SeedWidget = widget;
  }

private:
  vtkSeedRepresentation* SeedRepresentation = nullptr;
  vtkSeedWidget* SeedWidget = nullptr;
};
} // namespace

int main(int /* argc */, char* /* argv */[])
{
  vtkNew<vtkNamedColors> colors;

  std::array<double, 3> drawColor1{0, 0, 0};
  std::array<double, 3> drawColor2{0, 0, 0};
  auto color1 = colors->GetColor3ub("MidnightBlue").GetData();
  auto color2 = colors->GetColor3ub("Tomato").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }
  // Create an image
  vtkNew<vtkImageCanvasSource2D> drawing;
  drawing->SetScalarTypeToUnsignedChar();
  drawing->SetNumberOfScalarComponents(3);
  drawing->SetExtent(0, 20, 0, 50, 0, 0);
  // Make a blue background
  drawing->SetDrawColor(drawColor1.data());
  drawing->FillBox(0, 20, 0, 50);
  // Make a red circle
  drawing->SetDrawColor(drawColor2.data());
  drawing->DrawCircle(9, 10, 5);
  drawing->Update();

  vtkNew<vtkImageActor> imageActor;
  imageActor->SetInputData(drawing->GetOutput());

  // Create a renderer and render window
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(imageActor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("SeedWidgetImage");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Setup interactor style
  vtkNew<vtkInteractorStyleImage> interactorStyleImage;
  renderWindowInteractor->SetInteractorStyle(interactorStyleImage);

  // Create the representation
  vtkNew<vtkPointHandleRepresentation2D> handle;
  handle->GetProperty()->SetColor(1, 0, 0);
  vtkNew<vtkSeedRepresentation> rep;
  rep->SetHandleRepresentation(handle);

  // Seed widget
  vtkNew<vtkSeedWidget> seedWidget;
  seedWidget->SetInteractor(renderWindowInteractor);
  seedWidget->SetRepresentation(rep);

  vtkNew<vtkSeedImageCallback> seedCallback;
  seedCallback->SetRepresentation(rep);
  seedCallback->SetWidget(seedWidget);
  seedWidget->AddObserver(vtkCommand::PlacePointEvent, seedCallback);
  seedWidget->AddObserver(vtkCommand::InteractionEvent, seedCallback);

  renderWindow->Render();

  renderWindowInteractor->Initialize();
  renderWindow->Render();
  seedWidget->On();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
