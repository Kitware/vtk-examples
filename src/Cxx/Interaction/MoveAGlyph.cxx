#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkGlyph3D.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleTrackballActor.h>
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
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>

namespace {

// Define interaction style
class InteractorStyleMoveGlyph : public vtkInteractorStyleTrackballActor
{
public:
  static InteractorStyleMoveGlyph* New();
  vtkTypeMacro(InteractorStyleMoveGlyph, vtkInteractorStyleTrackballActor);

  InteractorStyleMoveGlyph()
  {
    this->MoveSphereSource = vtkSmartPointer<vtkSphereSource>::New();
    this->MoveSphereSource->SetRadius(.1);
    this->MoveSphereSource->Update();

    this->MoveMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    this->MoveMapper->SetInputConnection(
        this->MoveSphereSource->GetOutputPort());

    this->MoveActor = vtkSmartPointer<vtkActor>::New();
    this->MoveActor->SetMapper(this->MoveMapper);
    this->MoveActor->GetProperty()->SetColor(
        this->color->GetColor3d("Pink").GetData());
    // this->MoveActor->VisibilityOff();

    this->Move = false;
  }

  void OnMouseMove() override
  {
    if (!this->Move)
    {
      return;
    }

    vtkInteractorStyleTrackballActor::OnMouseMove();
  }

  void OnMiddleButtonUp() override
  {
    // Forward events
    vtkInteractorStyleTrackballActor::OnMiddleButtonUp();
    this->Move = false;
    this->MoveActor->VisibilityOff();

    this->Data->GetPoints()->SetPoint(this->SelectedPoint,
                                      this->MoveActor->GetPosition());
    this->Data->Modified();
    this->GetCurrentRenderer()->Render();
    this->GetCurrentRenderer()->GetRenderWindow()->Render();
  }
  void OnMiddleButtonDown() override
  {
    // Forward events
    vtkInteractorStyleTrackballActor::OnMiddleButtonDown();
    this->MoveActor->VisibilityOn();
    if (static_cast<vtkCellPicker*>(this->InteractionPicker)->GetPointId() >= 0)
    {
      vtkIdType id =
          dynamic_cast<vtkIdTypeArray*>(
              this->GlyphData->GetPointData()->GetArray("InputPointIds"))
              ->GetValue(static_cast<vtkCellPicker*>(this->InteractionPicker)
                             ->GetPointId());
      std::cout << "Id: " << id << std::endl;
      this->Move = true;
      this->SelectedPoint = id;

      double p[3];
      this->Data->GetPoint(id, p);
      std::cout << "p: " << p[0] << " " << p[1] << " " << p[2] << std::endl;
      this->MoveActor->SetPosition(p);
    }

    this->GetCurrentRenderer()->AddActor(this->MoveActor);
    this->InteractionProp = this->MoveActor;
  }
  vtkNew<vtkNamedColors> color;

  vtkPolyData* Data;
  vtkPolyData* GlyphData;

  vtkSmartPointer<vtkPolyDataMapper> MoveMapper;
  vtkSmartPointer<vtkActor> MoveActor;
  vtkSmartPointer<vtkSphereSource> MoveSphereSource;

  bool Move;
  vtkIdType SelectedPoint;
};
vtkStandardNewMacro(InteractorStyleMoveGlyph);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> color;

  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 0, 0);
  points->InsertNextPoint(2, 0, 0);

  vtkNew<vtkPolyData> input;
  input->SetPoints(points);

  vtkNew<vtkSphereSource> glyphSource;
  glyphSource->SetRadius(0.1);
  glyphSource->Update();

  vtkNew<vtkGlyph3D> glyph3D;
  glyph3D->GeneratePointIdsOn();
  glyph3D->SetSourceConnection(glyphSource->GetOutputPort());
  glyph3D->SetInputData(input);
  glyph3D->SetScaleModeToDataScalingOff();
  glyph3D->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(glyph3D->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(color->GetColor3d("Tomato").GetData());

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("MoveAGlyph");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(color->GetColor3d("Burlywood").GetData());

  renderWindow->Render();

  vtkNew<InteractorStyleMoveGlyph> style;
  renderWindowInteractor->SetInteractorStyle(style);
  style->Data = input;
  style->GlyphData = glyph3D->GetOutput();

  renderer->GetActiveCamera()->Zoom(0.9);
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
