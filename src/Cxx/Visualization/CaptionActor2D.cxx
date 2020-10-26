#include <vtkAbstractPicker.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCaptionActor2D.h>
#include <vtkCoordinate.h>
#include <vtkFollower.h>
#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVectorText.h>

#include <sstream>

namespace {
class MyStyle : public vtkInteractorStyleImage
{
public:
  static MyStyle* New();
  vtkTypeMacro(MyStyle, vtkInteractorStyleImage);

  std::vector<vtkActor2D*> Numbers;

  void OnLeftButtonDown()
  {
    // std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0]
    // << " " << this->Interactor->GetEventPosition()[1] << std::endl;
    this->Interactor->GetPicker()->Pick(
        this->Interactor->GetEventPosition()[0],
        this->Interactor->GetEventPosition()[1],
        0, // always zero.
        // this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
        this->CurrentRenderer);
    double picked[3];
    this->Interactor->GetPicker()->GetPickPosition(picked);
    // std::cout << "Picked point with coordinate: " << picked[0] << " " <<
    // picked[1] << " " << picked[2] << std::endl;

    this->AddNumber(picked);

    // Forward events
    vtkInteractorStyleImage::OnLeftButtonDown();

    // this->Interactor->GetRenderWindow()->Render();
    this->Interactor->Render();
  }

  void AddNumber(double p[3])
  {
    std::cout << "Adding marker at " << p[0] << " " << p[1]; //<< std::endl;

    // normally, with an image you would do
    // double* s = image->GetSpacing();
    // double* o = image->GetOrigin();
    // p[0] = static_cast<int>( (p[0] - o[0]) / s[0] + 0.5 );
    p[0] = static_cast<int>(p[0] + 0.5);
    p[1] = static_cast<int>(p[1] + 0.5);

    std::cout << " -> " << p[0] << " " << p[1] << std::endl;

    // Convert the current number to a string
    std::stringstream ss;
    ss << Numbers.size();
    std::cout << "Adding number: " << ss.str() << std::endl;

    // Create an actor for the text
    vtkNew<vtkCaptionActor2D> captionActor;
    captionActor->SetCaption(ss.str().c_str());
    captionActor->SetAttachmentPoint(p);
    captionActor->BorderOff();
    captionActor->GetCaptionTextProperty()->BoldOff();
    captionActor->GetCaptionTextProperty()->ItalicOff();
    captionActor->GetCaptionTextProperty()->ShadowOff();
    captionActor->ThreeDimensionalLeaderOff();

    this->CurrentRenderer->AddViewProp(captionActor);

    this->Numbers.push_back(captionActor);
  }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a blank, black image
  vtkNew<vtkImageCanvasSource2D> drawing;
  drawing->SetScalarTypeToUnsignedChar();
  drawing->SetNumberOfScalarComponents(3);
  drawing->SetExtent(0, 20, 0, 50, 0, 0);
  drawing->FillBox(0, 20, 0, 50);

  // Draw a red circle of radius 5 centered at (9,10)
  drawing->SetDrawColor(255, 0, 0, 0);
  drawing->DrawCircle(9, 10, 5);
  drawing->Update();

  vtkNew<vtkImageActor> actor;
  actor->GetMapper()->SetInputConnection(drawing->GetOutputPort());
  actor->InterpolateOff();

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("CaptionActor2D");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());

  renderer->GradientBackgroundOn();
  renderer->SetBackground2(colors->GetColor3d("Blue").GetData());

  renderWindow->Render();

  vtkNew<MyStyle> style;

  renderWindowInteractor->SetInteractorStyle(style);
  style->SetDefaultRenderer(renderer);
  style->SetCurrentRenderer(renderer);
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
