#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractSelection.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>
#include <vtkUnstructuredGrid.h>

namespace {

// Catch mouse events
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
  static MouseInteractorStyle* New();

  MouseInteractorStyle()
  {
    selectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    selectedActor = vtkSmartPointer<vtkActor>::New();
  }

  virtual void OnLeftButtonDown() override
  {
    vtkNew<vtkNamedColors> colors;

    // Get the location of the click (in window coordinates)
    int* pos = this->GetInteractor()->GetEventPosition();

    vtkNew<vtkCellPicker> picker;
    picker->SetTolerance(0.0005);

    // Pick from this location.
    picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());

    double* worldPosition = picker->GetPickPosition();
    std::cout << "Cell id is: " << picker->GetCellId() << std::endl;

    if (picker->GetCellId() != -1)
    {

      std::cout << "Pick position is: (" << worldPosition[0] << ", "
                << worldPosition[1] << ", " << worldPosition[2] << ")" << endl;

      vtkNew<vtkIdTypeArray> ids;
      ids->SetNumberOfComponents(1);
      ids->InsertNextValue(picker->GetCellId());

      vtkNew<vtkSelectionNode> selectionNode;
      selectionNode->SetFieldType(vtkSelectionNode::CELL);
      selectionNode->SetContentType(vtkSelectionNode::INDICES);
      selectionNode->SetSelectionList(ids);

      vtkNew<vtkSelection> selection;
      selection->AddNode(selectionNode);

      vtkNew<vtkExtractSelection> extractSelection;
      extractSelection->SetInputData(0, this->Data);
      extractSelection->SetInputData(1, selection);
      extractSelection->Update();

      // In selection
      vtkNew<vtkUnstructuredGrid> selected;
      selected->ShallowCopy(extractSelection->GetOutput());

      std::cout << "Number of points in the selection: "
                << selected->GetNumberOfPoints() << std::endl;
      std::cout << "Number of cells in the selection : "
                << selected->GetNumberOfCells() << std::endl;
      selectedMapper->SetInputData(selected);
      selectedActor->SetMapper(selectedMapper);
      selectedActor->GetProperty()->EdgeVisibilityOn();
      selectedActor->GetProperty()->SetColor(
          colors->GetColor3d("Tomato").GetData());

      selectedActor->GetProperty()->SetLineWidth(3);

      this->Interactor->GetRenderWindow()
          ->GetRenderers()
          ->GetFirstRenderer()
          ->AddActor(selectedActor);
    }
    // Forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  }

  vtkSmartPointer<vtkPolyData> Data;
  vtkSmartPointer<vtkDataSetMapper> selectedMapper;
  vtkSmartPointer<vtkActor> selectedActor;
};

vtkStandardNewMacro(MouseInteractorStyle);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPlaneSource> planeSource;
  planeSource->Update();

  vtkNew<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInputConnection(planeSource->GetOutputPort());
  triangleFilter->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(triangleFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->GetProperty()->SetColor(colors->GetColor3d("SeaGreen").GetData());
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("CellPicking");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->Initialize();

  // Set the custom stype to use for interaction.
  vtkNew<MouseInteractorStyle> style;
  style->SetDefaultRenderer(renderer);
  style->Data = triangleFilter->GetOutput();

  renderWindowInteractor->SetInteractorStyle(style);

  renderer->AddActor(actor);
  renderer->ResetCamera();

  renderer->SetBackground(colors->GetColor3d("PaleTurquoise").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
