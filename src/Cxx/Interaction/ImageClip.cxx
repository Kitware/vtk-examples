#include <vtkActor.h>
#include <vtkBorderRepresentation.h>
#include <vtkBorderWidget.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInformation.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkXMLPolyDataReader.h>

#include <string>

namespace {

class vtkBorderCallback2 : public vtkCommand
{
public:
  vtkBorderCallback2()
  {
  }

  static vtkBorderCallback2* New()
  {
    return new vtkBorderCallback2;
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {

    vtkBorderWidget* borderWidget = reinterpret_cast<vtkBorderWidget*>(caller);

    // Get the world coordinates of the two corners of the box
    auto lowerLeftCoordinate =
        static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
            ->GetPositionCoordinate();
    auto lowerLeft =
        lowerLeftCoordinate->GetComputedWorldValue(this->LeftRenderer);
    std::cout << "Lower left coordinate: " << lowerLeft[0] << ", "
              << lowerLeft[1] << ", " << lowerLeft[2] << std::endl;

    auto upperRightCoordinate =
        static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
            ->GetPosition2Coordinate();
    auto upperRight =
        upperRightCoordinate->GetComputedWorldValue(this->LeftRenderer);
    std::cout << "Upper right coordinate: " << upperRight[0] << ", "
              << upperRight[1] << ", " << upperRight[2] << std::endl;

    double* bounds = this->ImageActor->GetBounds();
    double xmin = bounds[0];
    double xmax = bounds[1];
    double ymin = bounds[2];
    double ymax = bounds[3];

    if ((lowerLeft[0] > xmin) && (upperRight[0] < xmax) &&
        (lowerLeft[1] > ymin) && (upperRight[1] < ymax))
    {
      this->ClipFilter->SetOutputWholeExtent(
          vtkMath::Round(lowerLeft[0]), vtkMath::Round(upperRight[0]),
          vtkMath::Round(lowerLeft[1]), vtkMath::Round(upperRight[1]), 0, 1);
    }
    else
    {
      std::cout << "box is NOT inside image" << std::endl;
    }
  }

  void SetLeftRenderer(vtkSmartPointer<vtkRenderer> renderer)
  {
    this->LeftRenderer = renderer;
  }
  void SetImageActor(vtkSmartPointer<vtkImageActor> actor)
  {
    this->ImageActor = actor;
  }
  void SetClipFilter(vtkSmartPointer<vtkImageClip> clip)
  {
    this->ClipFilter = clip;
  }

private:
  vtkSmartPointer<vtkRenderer> LeftRenderer;
  vtkSmartPointer<vtkImageActor> ImageActor;
  vtkSmartPointer<vtkImageClip> ClipFilter;
};

} // namespace

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> color;

  // Parse input arguments
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " Filename(.jpg) e.g. Gourds2.jpg"
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string inputFilename = argv[1];

  // Read the image
  vtkNew<vtkJPEGReader> jPEGReader;

  if (!jPEGReader->CanReadFile(inputFilename.c_str()))
  {
    std::cout << "Error: cannot read " << inputFilename << std::endl;
    return EXIT_FAILURE;
  }

  jPEGReader->SetFileName(inputFilename.c_str());
  jPEGReader->Update();

  int extent[6];
  jPEGReader->GetOutput()->GetExtent(extent);
  // xmin, xmax, ymin, ymax
  // std::cout << "extent: " << extent[0] << " " << extent[1] << " " <<
  // extent[2] << " " <<  extent[3] << " " <<  extent[4] << " " <<
  // extent[5] << std::endl;

  vtkNew<vtkImageActor> imageActor;
  imageActor->GetMapper()->SetInputConnection(jPEGReader->GetOutputPort());

  vtkNew<vtkRenderWindow> renderWindow;

  vtkNew<vtkRenderWindowInteractor> interactor;

  vtkNew<vtkInteractorStyleImage> style;
  interactor->SetInteractorStyle(style);

  vtkNew<vtkBorderWidget> borderWidget;
  borderWidget->SetInteractor(interactor);
  static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
      ->GetBorderProperty()
      ->SetColor(color->GetColor3d("Lime").GetData());
  borderWidget->SelectableOff();

  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges in normalized coordinates
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(color->GetColor3d("DarkSlateGray").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(color->GetColor3d("DimGray").GetData());

  leftRenderer->AddActor(imageActor);

  leftRenderer->ResetCamera();
  rightRenderer->ResetCamera();

  vtkNew<vtkImageClip> imageClip;
  imageClip->SetInputConnection(jPEGReader->GetOutputPort());
  jPEGReader->UpdateInformation();
  imageClip->SetOutputWholeExtent(jPEGReader->GetOutputInformation(0)->Get(
      vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
  imageClip->ClipDataOn();

  vtkNew<vtkImageActor> clipActor;
  clipActor->GetMapper()->SetInputConnection(imageClip->GetOutputPort());

  rightRenderer->AddActor(clipActor);

  vtkNew<vtkBorderCallback2> borderCallback;
  borderCallback->SetLeftRenderer(leftRenderer);
  borderCallback->SetImageActor(imageActor);
  borderCallback->SetClipFilter(imageClip);

  borderWidget->AddObserver(vtkCommand::InteractionEvent, borderCallback);

  renderWindow->SetWindowName("ImageClip");
  renderWindow->Render();
  borderWidget->On();
  interactor->Start();

  return EXIT_SUCCESS;
}
