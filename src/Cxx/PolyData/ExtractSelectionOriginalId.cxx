#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractSelection.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(50);
  pointSource->Update();

  std::cout << "There are " << pointSource->GetOutput()->GetNumberOfPoints()
            << " input points." << std::endl;

  vtkNew<vtkIdTypeArray> ids;
  ids->SetNumberOfComponents(1);

  // set values
  for (unsigned int i = 10; i < 20; i++)
  {
    ids->InsertNextValue(i);
  }

  vtkNew<vtkSelectionNode> selectionNode;
  selectionNode->SetFieldType(vtkSelectionNode::POINT);
  selectionNode->SetContentType(vtkSelectionNode::INDICES);
  selectionNode->SetSelectionList(ids);

  vtkNew<vtkSelection> selection;
  selection->AddNode(selectionNode);

  vtkNew<vtkExtractSelection> extractSelection;

  extractSelection->SetInputConnection(0, pointSource->GetOutputPort());
  extractSelection->SetInputData(1, selection);
  extractSelection->Update();

  // in selection
  vtkNew<vtkUnstructuredGrid> selected;
  selected->ShallowCopy(extractSelection->GetOutput());

  std::cout << "There are " << selected->GetNumberOfPoints()
            << " points in the selection." << std::endl;

  selected->Print(std::cout);

  vtkIdTypeArray* originalIds = dynamic_cast<vtkIdTypeArray*>(
      selected->GetPointData()->GetArray("vtkOriginalPointIds"));

  for (vtkIdType i = 0; i < originalIds->GetNumberOfTuples(); i++)
  {
    std::cout << "Point " << i << " was originally point "
              << originalIds->GetValue(i) << std::endl;
  }

  for (vtkIdType i = 0; i < originalIds->GetNumberOfTuples(); i++)
  {
    vtkIdType query = originalIds->GetValue(i);
    for (vtkIdType j = 0; j < originalIds->GetNumberOfTuples(); j++)
    {
      if (originalIds->GetValue(j) == query)
      {
        std::cout << "Original point " << query << " is now " << j << std::endl;
      }
    }
  }

  vtkNew<vtkDataSetMapper> inputMapper;
  inputMapper->SetInputConnection(pointSource->GetOutputPort());
  vtkNew<vtkActor> inputActor;
  inputActor->SetMapper(inputMapper);
  inputActor->GetProperty()->SetColor(
      colors->GetColor3d("MidnightBlue").GetData());
  inputActor->GetProperty()->SetPointSize(5);

  vtkNew<vtkDataSetMapper> selectedMapper;
  selectedMapper->SetInputData(selected);

  vtkNew<vtkActor> selectedActor;
  selectedActor->SetMapper(selectedMapper);
  selectedActor->GetProperty()->SetColor(
      colors->GetColor3d("MidnightBlue").GetData());
  selectedActor->GetProperty()->SetPointSize(5);

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("ExtractSelectionOriginalId");

  // And one interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Create a camera for all renderers
  vtkNew<vtkCamera> camera;

  // Setup the renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("BurlyWood").GetData());
  leftRenderer->SetActiveCamera(camera);

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());
  rightRenderer->SetActiveCamera(camera);

  leftRenderer->AddActor(inputActor);
  rightRenderer->AddActor(selectedActor);

  leftRenderer->ResetCamera();
  //rightRenderer->ResetCamera();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
