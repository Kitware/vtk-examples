#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>


#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

namespace {
// Define interaction style
class InteractorStyle : public vtkInteractorStyleRubberBandPick
{
public:
  static InteractorStyle* New();
  vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

  InteractorStyle()
  {
    this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    this->SelectedActor = vtkSmartPointer<vtkActor>::New();
    this->SelectedActor->SetMapper(SelectedMapper);
  }

  virtual void OnLeftButtonUp() override
  {
    vtkNew<vtkNamedColors> colors;

    // Forward events
    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

    vtkPlanes* frustum =
        static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())
            ->GetFrustum();

    vtkNew<vtkExtractGeometry> extractGeometry;
    extractGeometry->SetImplicitFunction(frustum);
    extractGeometry->SetInputData(this->Points);
    extractGeometry->Update();

    vtkNew<vtkVertexGlyphFilter> glyphFilter;
    glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
    glyphFilter->Update();

    vtkPolyData* selected = glyphFilter->GetOutput();
    std::cout << "Selected " << selected->GetNumberOfPoints() << " points."
              << std::endl;
    std::cout << "Selected " << selected->GetNumberOfCells() << " cells."
              << std::endl;
    this->SelectedMapper->SetInputData(selected);
    this->SelectedMapper->ScalarVisibilityOff();

    vtkIdTypeArray* ids = dynamic_cast<vtkIdTypeArray*>(
        selected->GetPointData()->GetArray("OriginalIds"));
    for (vtkIdType i = 0; i < ids->GetNumberOfTuples(); i++)
    {
      std::cout << "Id " << i << " : " << ids->GetValue(i) << std::endl;
    }

    this->SelectedActor->GetProperty()->SetColor(
        colors->GetColor3d("Red").GetData());
    this->SelectedActor->GetProperty()->SetPointSize(5);

    this->CurrentRenderer->AddActor(SelectedActor);
    this->GetInteractor()->GetRenderWindow()->Render();
    this->HighlightProp(NULL);
  }

  void SetPoints(vtkSmartPointer<vtkPolyData> points)
  {
    this->Points = points;
  }

private:
  vtkSmartPointer<vtkPolyData> Points;
  vtkSmartPointer<vtkActor> SelectedActor;
  vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
};

vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(20);
  pointSource->Update();

  vtkNew<vtkIdFilter> idFilter;
  idFilter->SetInputConnection(pointSource->GetOutputPort());
#if VTK890
  idFilter->SetCellIdsArrayName("OriginalIds");
  idFilter->SetPointIdsArrayName("OriginalIds");
#else
  idFilter->SetIdsArrayName("OriginalIds");
#endif
  idFilter->Update();

  vtkNew<vtkDataSetSurfaceFilter> surfaceFilter;
  surfaceFilter->SetInputConnection(idFilter->GetOutputPort());
  surfaceFilter->Update();

  vtkPolyData* input = surfaceFilter->GetOutput();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(input);
  mapper->ScalarVisibilityOff();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(3);
  actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("HighlightSelectedPoints");

  vtkNew<vtkAreaPicker> areaPicker;
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetPicker(areaPicker);
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  renderWindow->Render();

  vtkNew<InteractorStyle> style;
  style->SetPoints(input);
  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
