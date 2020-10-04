#include <vtkActor.h>
#include <vtkButtonWidget.h>
#include <vtkCommand.h>
#include <vtkCoordinate.h>
#include <vtkEllipticalButtonSource.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DButtonRepresentation.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSuperquadricSource.h>
#include <vtkTexture.h>

// Callback for the interaction
namespace {

class vtkButtonCallback : public vtkCommand
{
public:
  static vtkButtonCallback* New()
  {
    return new vtkButtonCallback;
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    auto buttonWidget = reinterpret_cast<vtkButtonWidget*>(caller);
    auto rep = reinterpret_cast<vtkProp3DButtonRepresentation*>(
        buttonWidget->GetRepresentation());
    int state = rep->GetState();
    this->Actor->GetProperty()->SetColor(
        reinterpret_cast<vtkActor*>(rep->GetButtonProp(state))
            ->GetProperty()
            ->GetColor());
    std::cout << "State: " << state << "\n";
  }

  vtkButtonCallback() : Actor(0)
  {
  }

  vtkActor* Actor;
};

vtkSmartPointer<vtkActor> CreateButtonActor(const char* textureFile);

} // namespace

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> color;

  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " imageFilename e.g. YinYang.png"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkSuperquadricSource> source;
  source->SetPhiResolution(64);
  source->SetThetaResolution(64);
  source->SetThetaRoundness(1.5);
  source->SetThickness(1.5);
  source->SetSize(2);
  source->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(source->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(color->GetColor3d("Burlywood").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(200, 200);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("EllipticalButton");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Create the widget and its representation
  vtkSmartPointer<vtkActor> buttonActor1 = CreateButtonActor(argv[1]);
  buttonActor1->GetProperty()->SetColor(color->GetColor3d("Tomato").GetData());
  vtkSmartPointer<vtkActor> buttonActor2 = CreateButtonActor(argv[1]);
  buttonActor2->GetProperty()->SetColor(color->GetColor3d("Banana").GetData());

  vtkNew<vtkProp3DButtonRepresentation> buttonRepresentation;
  buttonRepresentation->SetNumberOfStates(2);
  buttonRepresentation->SetButtonProp(0, buttonActor1);
  buttonRepresentation->SetButtonProp(1, buttonActor2);
  buttonRepresentation->FollowCameraOn();

  vtkNew<vtkButtonCallback> callback;
  callback->Actor = actor;
  actor->GetProperty()->SetColor(
      reinterpret_cast<vtkActor*>(buttonRepresentation->GetButtonProp(0))
          ->GetProperty()
          ->GetColor());

  renderWindowInteractor->Initialize();

  vtkNew<vtkButtonWidget> buttonWidget;
  buttonWidget->SetInteractor(renderWindowInteractor);
  buttonWidget->SetRepresentation(buttonRepresentation);
  buttonWidget->AddObserver(vtkCommand::StateChangedEvent, callback);
  buttonWidget->SetEnabled(1);

  renderer->ResetCamera();
  renderer->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

vtkSmartPointer<vtkActor> CreateButtonActor(const char* textureFile)
{
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> imageReader;
  imageReader.TakeReference(readerFactory->CreateImageReader2(textureFile));
  imageReader->SetFileName(textureFile);
  imageReader->Update();

  // Aspect ratio of image
  int dims[3];
  imageReader->GetOutput()->GetDimensions(dims);
  double aspect = static_cast<double>(dims[0]) / static_cast<double>(dims[1]);

  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(imageReader->GetOutputPort());

  vtkNew<vtkEllipticalButtonSource> ellipticalButtonSource;
  ellipticalButtonSource->SetCircumferentialResolution(50);
  ellipticalButtonSource->SetShoulderResolution(10);
  ellipticalButtonSource->SetTextureResolution(10);
  ellipticalButtonSource->SetRadialRatio(1.05);
  ellipticalButtonSource->SetShoulderTextureCoordinate(0.0, 0.0);
  ellipticalButtonSource->SetTextureDimensions(dims[0], dims[1]);
  ellipticalButtonSource->SetTextureStyleToProportional();
  ellipticalButtonSource->TwoSidedOn();
  ellipticalButtonSource->SetWidth(aspect);
  ellipticalButtonSource->SetHeight(1.0);
  ellipticalButtonSource->SetDepth(.15);
  ellipticalButtonSource->SetCenter(2, 2, 0);

  ellipticalButtonSource->SetOutputPointsPrecision(
      vtkAlgorithm::SINGLE_PRECISION);

  vtkNew<vtkPolyDataMapper> buttonMapper;
  buttonMapper->SetInputConnection(ellipticalButtonSource->GetOutputPort());

  vtkNew<vtkActor> buttonActor;
  buttonActor->SetMapper(buttonMapper);
  buttonActor->SetTexture(texture);

  return buttonActor;
}

} // namespace
