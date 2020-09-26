#include <vtkActor.h>
#include <vtkBorderRepresentation.h>
#include <vtkBorderWidget.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkInteractorStyleImage.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>

#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

class vtkBorderCallback : public vtkCommand
{
public:
  vtkBorderCallback()
  {
  }

  static vtkBorderCallback* New()
  {
    return new vtkBorderCallback;
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*);

  void SetRenderer(vtkSmartPointer<vtkRenderer> renderer)
  {
    this->Renderer = renderer;
  }
  void SetImageActor(vtkSmartPointer<vtkImageActor> actor)
  {
    this->ImageActor = actor;
  }

private:
  vtkSmartPointer<vtkRenderer> Renderer;
  vtkSmartPointer<vtkImageActor> ImageActor;
};

} // namespace

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0]
              << " Required parameters: Filename e.g. Ox.jpg" << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> imageReader;
  imageReader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  imageReader->SetFileName(argv[1]);
  imageReader->Update();

  // Shift the image center to (0,0)
  int dims[3];
  imageReader->GetOutput()->GetDimensions(dims);

  vtkNew<vtkImageChangeInformation> changeInformation;
  changeInformation->SetInputConnection(imageReader->GetOutputPort());
  changeInformation->CenterImageOn();
  changeInformation->Update();

  vtkSmartPointer<vtkImageData> image = changeInformation->GetOutput();

  vtkNew<vtkImageActor> imageActor;
  imageActor->GetMapper()->SetInputData(image);

  vtkNew<vtkRenderWindow> renderWindow;

  vtkNew<vtkRenderWindowInteractor> interactor;

  vtkNew<vtkInteractorStyleImage> style;
  interactor->SetInteractorStyle(style);

  vtkNew<vtkBorderWidget> borderWidget;
  borderWidget->SetInteractor(interactor);
  static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
      ->GetBorderProperty()
      ->SetColor(colors->GetColor3d("Chartreuse").GetData());
  borderWidget->SelectableOff();

  interactor->SetRenderWindow(renderWindow);

  // Setup both renderers
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("Peru").GetData());
  renderWindow->AddRenderer(renderer);

  renderer->AddActor(imageActor);

  renderer->ResetCamera();

  vtkNew<vtkBorderCallback> borderCallback;
  borderCallback->SetRenderer(renderer);
  borderCallback->SetImageActor(imageActor);

  borderWidget->AddObserver(vtkCommand::InteractionEvent, borderCallback);
  borderWidget->On();
  renderWindow->SetWindowName("CenterAnImage");
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {

void vtkBorderCallback::Execute(vtkObject* caller, unsigned long, void*)
{
  // Use this to format the output
  auto fmt = [](const double& x) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(2) << std::setw(8) << x;
    return os.str();
  };

  vtkBorderWidget* borderWidget = reinterpret_cast<vtkBorderWidget*>(caller);

  // Get the world coordinates of the two corners of the box
  vtkCoordinate* lowerLeftCoordinate =
      static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
          ->GetPositionCoordinate();
  double* lowerLeft =
      lowerLeftCoordinate->GetComputedWorldValue(this->Renderer);
  std::cout << "Lower left coordinate:  " << fmt(lowerLeft[0]) << ","
            << fmt(lowerLeft[1]) << "," << fmt(lowerLeft[2]) << std::endl;

  vtkCoordinate* upperRightCoordinate =
      static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
          ->GetPosition2Coordinate();
  double* upperRight =
      upperRightCoordinate->GetComputedWorldValue(this->Renderer);
  std::cout << "Upper right coordinate: " << fmt(upperRight[0]) << ","
            << fmt(upperRight[1]) << "," << fmt(upperRight[2]) << std::endl;

  double* bounds = this->ImageActor->GetBounds();
  double xmin = bounds[0];
  double xmax = bounds[1];
  double ymin = bounds[2];
  double ymax = bounds[3];

  if ((lowerLeft[0] > xmin) && (upperRight[0] < xmax) &&
      (lowerLeft[1] > ymin) && (upperRight[1] < ymax))
  {
    // std::cout << "box is inside image" << std::endl;
    // std::cout << "xmin: " << xmin << " xmax: " << xmax << " ymin: " << ymin
    // << " ymax: " << ymax << std::endl;
  }
  else
  {
    std::cout << "box is NOT inside image" << std::endl;
  }
}

} // namespace
