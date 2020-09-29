#include <sstream>
#include <vtkAbstractPicker.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
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
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVectorText.h>

#include <array>

namespace {

class MyStyle : public vtkInteractorStyleImage
{
public:
  static MyStyle* New();
  vtkTypeMacro(MyStyle, vtkInteractorStyleImage);

  std::vector<vtkActor2D*> Numbers;

  void OnLeftButtonDown() override
  {
    this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
                                        this->Interactor->GetEventPosition()[1],
                                        0, // always zero.
                                        this->CurrentRenderer);
    double picked[3];
    this->Interactor->GetPicker()->GetPickPosition(picked);
    this->AddNumber(picked);

    // Forward events
    vtkInteractorStyleImage::OnLeftButtonDown();

    // this->Interactor->GetRenderWindow()->Render();
    this->Interactor->Render();
  }

  void AddNumber(double p[3])
  {
    vtkNew<vtkNamedColors> colors;

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

    // Create an actor for the text
    vtkNew<vtkVectorText> textSource;
    textSource->SetText(ss.str().c_str());

    // get the bounds of the text
    textSource->Update();
    double* bounds = textSource->GetOutput()->GetBounds();
    // transform the polydata to be centered over the pick position
    double center[3] = {0.5 * (bounds[1] + bounds[0]),
                        0.5 * (bounds[3] + bounds[2]), 0.0};

    vtkNew<vtkTransform> trans;
    trans->Translate(-center[0], -center[1], 0);
    trans->Translate(p[0], p[1], 0);

    vtkNew<vtkTransformPolyDataFilter> tpd;
    tpd->SetTransform(trans);
    tpd->SetInputConnection(textSource->GetOutputPort());

    // Create a mapper
    vtkNew<vtkPolyDataMapper2D> mapper;
    vtkNew<vtkCoordinate> coordinate;
    coordinate->SetCoordinateSystemToWorld();
    mapper->SetTransformCoordinate(coordinate);
    mapper->SetInputConnection(tpd->GetOutputPort());

    vtkNew<vtkActor2D> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

    this->CurrentRenderer->AddViewProp(actor);
    this->Numbers.push_back(actor);
  }
};

vtkStandardNewMacro(MyStyle);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  std::array<double, 3> drawColor1{0, 0, 0};
  auto color1 = colors->GetColor3ub("DimGray").GetData();
  std::array<double, 3> drawColor2{0, 0, 0};
  auto color2 = colors->GetColor3ub("HotPink").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }

  // Create a blank, black image
  vtkNew<vtkImageCanvasSource2D> drawing;
  drawing->SetScalarTypeToUnsignedChar();
  drawing->SetNumberOfScalarComponents(3);
  drawing->SetExtent(0, 20, 0, 50, 0, 0);
  drawing->SetDrawColor(drawColor1.data());
  drawing->FillBox(0, 20, 0, 50);

  // Draw a circle of radius 5 centered at (9,10)
  drawing->SetDrawColor(drawColor2.data());
  drawing->DrawCircle(9, 10, 5);
  drawing->Update();

  vtkNew<vtkImageActor> actor;
  actor->GetMapper()->SetInputConnection(drawing->GetOutputPort());
  actor->InterpolateOff();

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("MarkKeypoints");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SkyBlue").GetData());

  renderer->GradientBackgroundOn();
  renderer->SetBackground2(colors->GetColor3d("MidnightBlue").GetData());

  renderWindow->Render();

  vtkNew<MyStyle> style;

  renderWindowInteractor->SetInteractorStyle(style);
  style->SetDefaultRenderer(renderer);
  style->SetCurrentRenderer(renderer);
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
