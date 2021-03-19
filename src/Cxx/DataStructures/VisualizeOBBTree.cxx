#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBBTree.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>
#include <vtkWidgetEvent.h>
#include <vtkWidgetEventTranslator.h>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

#include <vtksys/SystemTools.hxx>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

namespace {
class vtkSliderCallback : public vtkCommand
{
public:
  static vtkSliderCallback* New()
  {
    return new vtkSliderCallback;
  }

  vtkSliderCallback() : OBBTree(0), Level(0), PolyData(0), Renderer(0)
  {
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    this->Level = vtkMath::Round(
        static_cast<vtkSliderRepresentation*>(sliderWidget->GetRepresentation())
            ->GetValue());
    this->OBBTree->GenerateRepresentation(this->Level, this->PolyData);
    this->Renderer->Render();
  }

  vtkOBBTree* OBBTree;
  int Level;
  vtkPolyData* PolyData;
  vtkRenderer* Renderer;
};
} // namespace

int main(int argc, char* argv[])
{

  auto polyData = ReadPolyData(argc > 1 ? argv[1] : "");
  ;

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> pointsMapper;
  pointsMapper->SetInputData(polyData);
  pointsMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> pointsActor;
  pointsActor->SetMapper(pointsMapper);
  pointsActor->GetProperty()->SetInterpolationToFlat();
  pointsActor->GetProperty()->SetColor(colors->GetColor4d("Yellow").GetData());
  pointsActor->GetProperty()->SetOpacity(.3);

  int maxLevel = 5;
  // Create the tree
  vtkNew<vtkOBBTree> obbTree;
  obbTree->SetDataSet(polyData);
  obbTree->SetMaxLevel(maxLevel);
  obbTree->BuildLocator();

  double corner[3] = {0.0, 0.0, 0.0};
  double max[3] = {0.0, 0.0, 0.0};
  double mid[3] = {0.0, 0.0, 0.0};
  double min[3] = {0.0, 0.0, 0.0};
  double size[3] = {0.0, 0.0, 0.0};

  obbTree->ComputeOBB(polyData, corner, max, mid, min, size);

  std::cout << "Corner:\t" << corner[0] << ", " << corner[1] << ", "
            << corner[2] << std::endl
            << "Max:\t" << max[0] << ", " << max[1] << ", " << max[2]
            << std::endl
            << "Mid:\t" << mid[0] << ", " << mid[1] << ", " << mid[2]
            << std::endl
            << "Min:\t" << min[0] << ", " << min[1] << ", " << min[2]
            << std::endl
            << "Size:\t" << size[0] << ", " << size[1] << ", " << size[2]
            << std::endl;

  // Initialize the representation
  vtkNew<vtkPolyData> polydata;
  obbTree->GenerateRepresentation(0, polydata);

  vtkNew<vtkPolyDataMapper> obbtreeMapper;
  obbtreeMapper->SetInputData(polydata);

  vtkNew<vtkActor> obbtreeActor;
  obbtreeActor->SetMapper(obbtreeMapper);
  obbtreeActor->GetProperty()->SetInterpolationToFlat();
  obbtreeActor->GetProperty()->SetOpacity(.5);
  obbtreeActor->GetProperty()->EdgeVisibilityOn();
  obbtreeActor->GetProperty()->SetColor(
      colors->GetColor4d("SpringGreen").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  // An interactor
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->SetInteractorStyle(style);

  // Add the actors to the scene
  renderer->AddActor(pointsActor);
  renderer->AddActor(obbtreeActor);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
  renderer->UseHiddenLineRemovalOn();

  // Render an image (lights and cameras are created automatically)
  renderWindow->SetWindowName("VisualizeOBBTree");
  renderWindow->SetSize(600, 600);
  renderWindow->Render();

  vtkNew<vtkSliderRepresentation2D> sliderRep;
  sliderRep->SetMinimumValue(0);
  sliderRep->SetMaximumValue(obbTree->GetLevel());
  sliderRep->SetValue(obbTree->GetLevel() / 2);
  sliderRep->SetTitleText("Level");
  sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  sliderRep->GetPoint1Coordinate()->SetValue(.2, .2);
  sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  sliderRep->GetPoint2Coordinate()->SetValue(.8, .2);
  sliderRep->SetSliderLength(0.075);
  sliderRep->SetSliderWidth(0.05);
  sliderRep->SetEndCapLength(0.05);
  sliderRep->GetTitleProperty()->SetColor(
      colors->GetColor3d("Beige").GetData());
  sliderRep->GetCapProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());
  sliderRep->GetSliderProperty()->SetColor(
      colors->GetColor3d("LightBlue").GetData());
  sliderRep->GetSelectedProperty()->SetColor(
      colors->GetColor3d("Violet").GetData());

  vtkNew<vtkSliderWidget> sliderWidget;
  sliderWidget->SetInteractor(renderWindowInteractor);
  sliderWidget->SetRepresentation(sliderRep);
  sliderWidget->SetAnimationModeToAnimate();
  sliderWidget->EnabledOn();

  vtkNew<vtkSliderCallback> callback;
  callback->OBBTree = obbTree;
  callback->PolyData = polydata;
  callback->Renderer = renderer;
  callback->Execute(sliderWidget, 0, 0);

  sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);

  renderWindowInteractor->Initialize();
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension =
      vtksys::SystemTools::GetFilenameLastExtension(std::string(fileName));
  if (extension == ".ply")
  {
    auto reader = vtkSmartPointer<vtkPLYReader>::New();
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtp")
  {
    auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".obj")
  {
    auto reader = vtkSmartPointer<vtkOBJReader>::New();
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".stl")
  {
    auto reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtk")
  {
    auto reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".g")
  {
    auto reader = vtkSmartPointer<vtkBYUReader>::New();
    reader->SetGeometryFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    // Return a polydata sphere if the extension is unknown.
    auto source = vtkSmartPointer<vtkSphereSource>::New();
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}
} // namespace
