#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkColorSeries.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractSelection.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkUnstructuredGrid.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // A nifty way of setting backgrounds in multiple renderers
  vtkNew<vtkColorSeries> colorSeries;
  colorSeries->SetColorSchemeByName("Brewer Qualitative Pastel2");
  colors->SetColor("ren0", colorSeries->GetColor(0));
  colors->SetColor("ren1", colorSeries->GetColor(1));
  colors->SetColor("ren2", colorSeries->GetColor(2));
  colors->SetColor("ren3", colorSeries->GetColor(3));

  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(50);
  pointSource->Update();

  std::cout << "There are " << pointSource->GetOutput()->GetNumberOfPoints()
            << " input points." << std::endl;

  vtkNew<vtkIdTypeArray> ids1;
  ids1->SetNumberOfComponents(1);

  // Set values
  for (unsigned int i = 10; i < 20; i++)
  {
    ids1->InsertNextValue(i);
  }

  vtkNew<vtkIdTypeArray> ids2;
  ids2->SetNumberOfComponents(1);

  for (unsigned int i = 15; i < 30; i++)
  {
    ids2->InsertNextValue(i);
  }

  vtkNew<vtkSelectionNode> selectionNode1;
  selectionNode1->SetFieldType(vtkSelectionNode::POINT);
  selectionNode1->SetContentType(vtkSelectionNode::INDICES);
  selectionNode1->SetSelectionList(ids1);

  vtkNew<vtkSelectionNode> selectionNode2;
  selectionNode2->SetFieldType(vtkSelectionNode::POINT);
  selectionNode2->SetContentType(vtkSelectionNode::INDICES);
  selectionNode2->SetSelectionList(ids2);

  vtkNew<vtkSelection> selection1;
  selection1->AddNode(selectionNode1);

  vtkNew<vtkSelection> selection2;
  selection2->AddNode(selectionNode2);

  vtkNew<vtkSelection> selectionCombined;
  selectionCombined->AddNode(selectionNode1);
  selectionCombined->Union(selectionNode2);

  std::cout << "There are " << selectionCombined->GetNumberOfNodes()
            << " nodes." << std::endl;

  vtkNew<vtkExtractSelection> extractSelection1;
  extractSelection1->SetInputConnection(0, pointSource->GetOutputPort());
  extractSelection1->SetInputData(1, selection1);
  extractSelection1->Update();

  vtkNew<vtkExtractSelection> extractSelection2;
  extractSelection2->SetInputConnection(0, pointSource->GetOutputPort());
  extractSelection2->SetInputData(1, selection2);
  extractSelection2->Update();

  vtkNew<vtkExtractSelection> extractSelectionCombined;
  extractSelectionCombined->SetInputConnection(0, pointSource->GetOutputPort());
  extractSelectionCombined->SetInputData(1, selectionCombined);
  extractSelectionCombined->Update();

  // In selection
  vtkDataSet* ds =
      dynamic_cast<vtkDataSet*>(extractSelectionCombined->GetOutput());
  std::cout << "There are " << ds->GetNumberOfPoints()
            << " points in the selection." << std::endl;
  std::cout << "There are " << ds->GetNumberOfCells()
            << " cells in the selection." << std::endl;

  /*
  // Not in selection
  selectionNode1->GetProperties()->Set(vtkSelectionNode::INVERSE(), 1); //invert
  the selection
  selectionNode2->GetProperties()->Set(vtkSelectionNode::INVERSE(), 1); //invert
  the selection extractSelection->Update(); std::cout << "There are " <<
  dynamic_cast<vtkDataSet*> (extractSelection->GetOutput())->GetNumberOfPoints()
            << " points not in the selection." << std::endl;
  */

  // Visualize
  vtkNew<vtkDataSetMapper> mapperOriginal;
  mapperOriginal->SetInputConnection(pointSource->GetOutputPort());
  mapperOriginal->ScalarVisibilityOff();

  vtkNew<vtkDataSetMapper> mapper1;
  mapper1->SetInputConnection(extractSelection1->GetOutputPort());
  mapper1->ScalarVisibilityOff();

  vtkNew<vtkDataSetMapper> mapper2;
  mapper2->SetInputConnection(extractSelection2->GetOutputPort());
  mapper2->ScalarVisibilityOff();

  vtkNew<vtkDataSetMapper> mapperCombined;
  mapperCombined->SetInputConnection(extractSelectionCombined->GetOutputPort());
  mapperCombined->ScalarVisibilityOff();

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(mapperOriginal);
  originalActor->GetProperty()->SetPointSize(5);
  originalActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  vtkNew<vtkActor> actor1;
  actor1->SetMapper(mapper1);
  actor1->GetProperty()->SetPointSize(5);
  actor1->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  vtkNew<vtkActor> actor2;
  actor2->SetMapper(mapper2);
  actor2->GetProperty()->SetPointSize(5);
  actor2->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  vtkNew<vtkActor> actorCombined;
  actorCombined->SetMapper(mapperCombined);
  actorCombined->GetProperty()->SetPointSize(5);
  actorCombined->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(1000, 250);
  renderWindow->SetWindowName("MergeSelections");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  double originalViewport[4] = {0.0, 0.0, 0.25, 1.0};
  double selection1Viewport[4] = {0.25, 0.0, 0.5, 1.0};
  double selection2Viewport[4] = {0.5, 0.0, 0.75, 1.0};
  double selectionCombinedViewport[4] = {0.75, 0.0, 1.0, 1.0};

  // Create a camera for all renderers
  vtkNew<vtkCamera> camera;

  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->SetBackground(colors->GetColor3d("Ren0").GetData());
  originalRenderer->SetActiveCamera(camera);

  vtkNew<vtkRenderer> selection1Renderer;
  selection1Renderer->SetViewport(selection1Viewport);
  selection1Renderer->SetBackground(colors->GetColor3d("Ren1").GetData());
  selection1Renderer->SetActiveCamera(camera);

  vtkNew<vtkRenderer> selection2Renderer;
  selection2Renderer->SetViewport(selection2Viewport);
  selection2Renderer->SetBackground(colors->GetColor3d("Ren2").GetData());
  selection2Renderer->SetActiveCamera(camera);

  vtkNew<vtkRenderer> selectionCombinedRenderer;
  selectionCombinedRenderer->SetViewport(selectionCombinedViewport);
  selectionCombinedRenderer->SetBackground(
      colors->GetColor3d("Ren3").GetData());
  selectionCombinedRenderer->SetActiveCamera(camera);

  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(selection1Renderer);
  renderWindow->AddRenderer(selection2Renderer);
  renderWindow->AddRenderer(selectionCombinedRenderer);

  originalRenderer->AddActor(originalActor);
  selection1Renderer->AddActor(actor1);
  selection2Renderer->AddActor(actor2);
  selectionCombinedRenderer->AddActor(actorCombined);

  originalRenderer->ResetCamera();

  renderWindow->Render();

  renderWindowInteractor->Start();
  return EXIT_SUCCESS;
}
