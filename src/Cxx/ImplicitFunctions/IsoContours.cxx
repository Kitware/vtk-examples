#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCommand.h>
#include <vtkContourFilter.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleUser.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSliderRepresentation.h>
#include <vtkSliderRepresentation3D.h>
#include <vtkSliderWidget.h>
#include <vtkXMLImageDataWriter.h>

namespace {
void CreateData(vtkImageData* data);

class vtkSliderCallback : public vtkCommand
{
public:
  static vtkSliderCallback* New()
  {
    return new vtkSliderCallback;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value =
        static_cast<vtkSliderRepresentation*>(sliderWidget->GetRepresentation())
            ->GetValue();
    this->ContourFilter->GenerateValues(1, value, value);
  }
  vtkSliderCallback() : ContourFilter(NULL)
  {
  }
  vtkContourFilter* ContourFilter;
};
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkImageData> data;
  CreateData(data);

  // Create an isosurface
  vtkNew<vtkContourFilter> contourFilter;
  contourFilter->SetInputData(data);
  contourFilter->GenerateValues(1, 10,
                                10); // (numContours, rangeStart, rangeEnd)

  // Map the contours to graphical primitives
  vtkNew<vtkPolyDataMapper> contourMapper;
  contourMapper->SetInputConnection(contourFilter->GetOutputPort());

  // Create an actor for the contours
  vtkNew<vtkActor> contourActor;
  contourActor->SetMapper(contourMapper);
  contourActor->GetProperty()->SetLineWidth(5);

  // Create the outline
  vtkNew<vtkOutlineFilter> outlineFilter;
  outlineFilter->SetInputData(data);

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outlineFilter->GetOutputPort());
  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("Gray").GetData());
  outlineActor->GetProperty()->SetLineWidth(3);

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("IsoContours");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(contourActor);
  renderer->AddActor(outlineActor);
  renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());

  vtkNew<vtkSliderRepresentation3D> sliderRep;
  sliderRep->SetMinimumValue(0.0);
  sliderRep->SetMaximumValue(30.0);
  sliderRep->SetValue(10.0);
  sliderRep->SetTitleText("Contour value");
  sliderRep->SetPoint1InWorldCoordinates(-20, -40, 0);
  sliderRep->SetPoint2InWorldCoordinates(0, -40, 0);
  sliderRep->SetSliderWidth(.2);
  sliderRep->SetLabelHeight(.1);

  vtkNew<vtkSliderWidget> sliderWidget;
  sliderWidget->SetInteractor(interactor);
  sliderWidget->SetRepresentation(sliderRep);
  sliderWidget->SetAnimationModeToAnimate();
  sliderWidget->EnabledOn();

  vtkNew<vtkSliderCallback> callback;
  callback->ContourFilter = contourFilter;

  sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);

  vtkNew<vtkInteractorStyleUser> style;
  interactor->SetInteractorStyle(style);

  renderWindow->SetSize(500, 500);
  renderWindow->Render();
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;

  return EXIT_SUCCESS;
}
namespace {
void CreateData(vtkImageData* data)
{
  data->SetExtent(-25, 25, -25, 25, 0, 0);
  data->AllocateScalars(VTK_DOUBLE, 1);
  int* extent = data->GetExtent();

  for (int y = extent[2]; y <= extent[3]; y++)
  {
    for (int x = extent[0]; x <= extent[1]; x++)
    {
      double* pixel = static_cast<double*>(data->GetScalarPointer(x, y, 0));
      pixel[0] = sqrt(pow(x, 2.0) + pow(y, 2.0));
    }
  }

  vtkNew<vtkXMLImageDataWriter> writer;
  writer->SetFileName("data.vti");
  writer->SetInputData(data);
  writer->Write();
}
} // namespace
