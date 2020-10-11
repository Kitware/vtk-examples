#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellIterator.h>
#include <vtkCleanPolyData.h>
#include <vtkCommand.h>
#include <vtkDataSetMapper.h>
#include <vtkDelaunay3D.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <vtkSmartPointer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnstructuredGrid.h>

#include <sstream>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>
#include <vtksys/SystemTools.hxx>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);

void MakeWidget(vtkSliderWidget*, vtkDelaunay3D*, vtkTextMapper*, vtkRenderer*,
                vtkRenderWindowInteractor*);
} // namespace

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " filename.vtp" << std::endl;
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkPolyData> polyData = ReadPolyData(argc > 1 ? argv[1] : "");

  vtkNew<vtkNamedColors> color;

  vtkNew<vtkDataSetMapper> originalMapper;
  originalMapper->SetInputData(polyData);
  originalMapper->ScalarVisibilityOff();
  originalMapper->SetScalarModeToUseCellData();

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetColor(color->GetColor3d("tomato").GetData());
  originalActor->GetProperty()->SetInterpolationToFlat();

  // Clean the polydata. This will remove duplicate points that may be
  // present in the input data.
  vtkNew<vtkCleanPolyData> cleaner;
  cleaner->SetInputData(polyData);

  // Generate a tetrahedral mesh from the input points. By
  // default, the generated volume is the convex hull of the points.
  vtkNew<vtkDelaunay3D> delaunay3D;
  delaunay3D->SetInputConnection(cleaner->GetOutputPort());

  vtkNew<vtkDataSetMapper> delaunayMapper;
  delaunayMapper->SetInputConnection(delaunay3D->GetOutputPort());
  delaunayMapper->SetScalarModeToUseCellData();

  vtkNew<vtkActor> delaunayActor;
  delaunayActor->SetMapper(delaunayMapper);
  delaunayActor->GetProperty()->SetColor(color->GetColor3d("banana").GetData());
  delaunayActor->GetProperty()->SetInterpolationToFlat();

  // Generate a mesh from the input points. If Alpha is non-zero, then
  // tetrahedra, triangles, edges and vertices that lie within the
  // alpha radius are output.
  vtkNew<vtkDelaunay3D> delaunay3DAlpha;
  delaunay3DAlpha->SetInputConnection(cleaner->GetOutputPort());
  delaunay3DAlpha->SetAlpha(0.0105);
  delaunay3DAlpha->Update();

  vtkNew<vtkUnsignedCharArray> cellData;
  cellData->SetNumberOfComponents(3);

  int numTetras = 0;
  int numLines = 0;
  int numTris = 0;
  int numVerts = 0;

  auto it = delaunay3DAlpha->GetOutput()->NewCellIterator();
  for (it->InitTraversal(); !it->IsDoneWithTraversal(); it->GoToNextCell())
  {
    if (it->GetCellType() == VTK_TETRA)
    {
      numTetras++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Banana").GetData());
    }
    else if (it->GetCellType() == VTK_LINE)
    {
      numLines++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Peacock").GetData());
    }
    else if (it->GetCellType() == VTK_TRIANGLE)
    {
      numTris++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Tomato").GetData());
    }
    else if (it->GetCellType() == VTK_VERTEX)
    {
      numVerts++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Lime").GetData());
    }
  }
  it->Delete();

  std::stringstream ss;
  ss << "numTetras: " << numTetras << std::endl;
  ss << "numLines: " << numLines << std::endl;
  ss << "numTris: " << numTris << std::endl;
  ss << "numVerts: " << numVerts;

  // Set the cell color depending on cell type
  delaunay3DAlpha->GetOutput()->GetCellData()->SetScalars(cellData);

  vtkNew<vtkDataSetMapper> delaunayAlphaMapper;
  delaunayAlphaMapper->SetInputConnection(delaunay3DAlpha->GetOutputPort());
  delaunayAlphaMapper->SetScalarModeToUseCellData();

  vtkNew<vtkActor> delaunayAlphaActor;
  delaunayAlphaActor->SetMapper(delaunayAlphaMapper);
  delaunayAlphaActor->GetProperty()->SetPointSize(5.0);
  delaunayAlphaActor->GetProperty()->SetInterpolationToFlat();

  // Visualize

  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetFontSize(16);
  textProperty->SetColor(color->GetColor3d("Black").GetData());

  vtkNew<vtkTextMapper> textMapper;
  textMapper->SetInput(ss.str().c_str());
  textMapper->SetTextProperty(textProperty);

  vtkNew<vtkActor2D> textActor;
  textActor->SetMapper(textMapper);
  textActor->SetPosition(10, 10);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.33, 1.0};
  double centerViewport[4] = {0.33, 0.0, 0.66, 1.0};
  double rightViewport[4] = {0.66, 0.0, 1.0, 1.0};

  // Shared camera
  vtkNew<vtkCamera> sharedCamera;

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetActiveCamera(sharedCamera);
  originalRenderer->UseHiddenLineRemovalOn();

  vtkNew<vtkRenderer> delaunayRenderer;
  delaunayRenderer->SetActiveCamera(sharedCamera);
  delaunayRenderer->UseHiddenLineRemovalOn();

  vtkNew<vtkRenderer> delaunayAlphaRenderer;
  delaunayAlphaRenderer->SetActiveCamera(sharedCamera);
  delaunayAlphaRenderer->UseHiddenLineRemovalOn();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(900, 300);
  renderWindow->SetWindowName("Delaunay3DDemo");

  renderWindow->AddRenderer(originalRenderer);
  originalRenderer->SetViewport(leftViewport);

  renderWindow->AddRenderer(delaunayRenderer);
  delaunayRenderer->SetViewport(centerViewport);

  renderWindow->AddRenderer(delaunayAlphaRenderer);
  delaunayAlphaRenderer->SetViewport(rightViewport);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkSliderWidget> widget;
  MakeWidget(widget, delaunay3DAlpha, textMapper, delaunayAlphaRenderer,
             renderWindowInteractor);

  originalRenderer->AddActor(originalActor);
  delaunayRenderer->AddActor(delaunayActor);
  delaunayAlphaRenderer->AddActor(delaunayAlphaActor);
  delaunayAlphaRenderer->AddViewProp(textActor);

  originalRenderer->SetBackground(color->GetColor3d("Slate_Grey").GetData());
  delaunayRenderer->SetBackground(color->GetColor3d("Light_Grey").GetData());
  delaunayAlphaRenderer->SetBackground(color->GetColor3d("Grey").GetData());

  originalRenderer->ResetCamera();
  renderWindow->Render();

  // Render and interact
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
// These callbacks do the actual work.
// Callbacks for the interactions
class SliderCallbackAlpha : public vtkCommand
{
public:
  static SliderCallbackAlpha* New()
  {
    return new SliderCallbackAlpha;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    this->Delaunay3D->SetAlpha(value);
    this->Delaunay3D->Update();

    vtkNew<vtkUnsignedCharArray> cellData;
    cellData->SetNumberOfComponents(3);

    // Set the cell color depending on cell type
    vtkNew<vtkNamedColors> color;
    int numTetras = 0;
    int numLines = 0;
    int numTris = 0;
    int numVerts = 0;

    auto it = this->Delaunay3D->GetOutput()->NewCellIterator();
    for (it->InitTraversal(); !it->IsDoneWithTraversal(); it->GoToNextCell())
    {
      if (it->GetCellType() == VTK_TETRA)
      {
        numTetras++;
        cellData->InsertNextTypedTuple(color->GetColor3ub("Banana").GetData());
      }
      else if (it->GetCellType() == VTK_LINE)
      {
        numLines++;
        cellData->InsertNextTypedTuple(color->GetColor3ub("Peacock").GetData());
      }
      else if (it->GetCellType() == VTK_TRIANGLE)
      {
        numTris++;
        cellData->InsertNextTypedTuple(color->GetColor3ub("Tomato").GetData());
      }
      else if (it->GetCellType() == VTK_VERTEX)
      {
        numVerts++;
        cellData->InsertNextTypedTuple(color->GetColor3ub("Lime").GetData());
      }
    }
    it->Delete();

    this->Delaunay3D->GetOutput()->GetCellData()->SetScalars(cellData);
    std::stringstream ss;
    ss << "numTetras: " << numTetras << std::endl;
    ss << "numLines: " << numLines << std::endl;
    ss << "numTris: " << numTris << std::endl;
    ss << "numVerts: " << numVerts;
    this->TextMapper->SetInput(ss.str().c_str());
  }
  SliderCallbackAlpha() : Delaunay3D(0), TextMapper(0)
  {
  }
  vtkDelaunay3D* Delaunay3D;
  vtkTextMapper* TextMapper;
};

void MakeWidget(vtkSliderWidget* widget, vtkDelaunay3D* delaunay3D,
                vtkTextMapper* textMapper, vtkRenderer* renderer,
                vtkRenderWindowInteractor* interactor)
{
  // Setup a slider widget for each varying parameter
  double tubeWidth(.02);
  double sliderLength(.04);
  double titleHeight(.04);
  double labelHeight(.04);

  vtkNew<vtkSliderRepresentation2D> sliderRepAlpha;

  sliderRepAlpha->SetRenderer(renderer);

  sliderRepAlpha->SetMinimumValue(0.0001);
  sliderRepAlpha->SetMaximumValue(1.0);
  sliderRepAlpha->SetValue(delaunay3D->GetAlpha());
  sliderRepAlpha->SetTitleText("Alpha");
  sliderRepAlpha->SetRenderer(renderer);
  sliderRepAlpha->GetPoint1Coordinate()->SetValue(0.1, .1);
  sliderRepAlpha->GetPoint1Coordinate()
      ->SetCoordinateSystemToNormalizedViewport();
  sliderRepAlpha->GetPoint2Coordinate()->SetValue(.9, .1);
  sliderRepAlpha->GetPoint2Coordinate()
      ->SetCoordinateSystemToNormalizedViewport();

  sliderRepAlpha->SetTubeWidth(tubeWidth);
  sliderRepAlpha->SetSliderLength(sliderLength);
  sliderRepAlpha->SetTitleHeight(titleHeight);
  sliderRepAlpha->SetLabelHeight(labelHeight);
  sliderRepAlpha->SetEndCapLength(tubeWidth * 1.5);
  sliderRepAlpha->SetSliderWidth(tubeWidth * 1.5);
  sliderRepAlpha->BuildRepresentation();

  widget->SetRepresentation(sliderRepAlpha);
  widget->SetAnimationModeToAnimate();
  widget->SetNumberOfAnimationSteps(2);
  widget->SetInteractor(interactor);
  widget->EnabledOn();

  vtkNew<SliderCallbackAlpha> callbackAlpha;
  callbackAlpha->Delaunay3D = delaunay3D;
  callbackAlpha->TextMapper = textMapper;

  widget->AddObserver(vtkCommand::InteractionEvent, callbackAlpha);
}

vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension =
      vtksys::SystemTools::GetFilenameExtension(std::string(fileName));
  if (extension == ".ply")
  {
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtp")
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".obj")
  {
    vtkNew<vtkOBJReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".stl")
  {
    vtkNew<vtkSTLReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtk")
  {
    vtkNew<vtkPolyDataReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".g")
  {
    vtkNew<vtkBYUReader> reader;
    reader->SetGeometryFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkSphereSource> source;
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}
} // namespace
