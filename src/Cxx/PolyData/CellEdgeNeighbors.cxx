#include <vtkActor.h>
#include <vtkCell.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractSelection.h>
#include <vtkIdList.h>
#include <vtkIdTypeArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkSphereSource.h>
#include <vtkTriangleFilter.h>

#include <list>

int main(int, char*[])
{
  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInputConnection(sphereSource->GetOutputPort());
  triangleFilter->Update();

  vtkIdType cellId = 0;

  vtkNew<vtkIdList> cellPointIds;
  triangleFilter->GetOutput()->GetCellPoints(cellId, cellPointIds);

  std::list<vtkIdType> neighbors;

  for (vtkIdType i = 0; i < cellPointIds->GetNumberOfIds(); i++)
  {
    vtkNew<vtkIdList> idList;

    // add one of the edge points
    idList->InsertNextId(cellPointIds->GetId(i));

    // add the other edge point
    if (i + 1 == cellPointIds->GetNumberOfIds())
    {
      idList->InsertNextId(cellPointIds->GetId(0));
    }
    else
    {
      idList->InsertNextId(cellPointIds->GetId(i + 1));
    }

    // get the neighbors of the cell
    vtkNew<vtkIdList> neighborCellIds;

    triangleFilter->GetOutput()->GetCellNeighbors(cellId, idList,
                                                  neighborCellIds);

    for (vtkIdType j = 0; j < neighborCellIds->GetNumberOfIds(); j++)
    {
      neighbors.push_back(neighborCellIds->GetId(j));
    }
  }

  std::cout << "Edge neighbor ids are: " << std::endl;

  for (auto it1 = neighbors.begin(); it1 != neighbors.end(); ++it1)
  {
    std::cout << " " << *it1;
  }
  std::cout << std::endl;

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkDataSetMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  sphereMapper->SetResolveCoincidentTopologyToPolygonOffset();

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetEdgeColor(
      colors->GetColor3d("Lamp_Black").GetData());
  sphereActor->GetProperty()->EdgeVisibilityOn();
  sphereActor->GetProperty()->SetLineWidth(5);

  vtkNew<vtkDataSetMapper> mainCellMapper;

  vtkNew<vtkDataSetMapper> neighborCellsMapper;

  // Create a dataset with the cell of interest
  {
    vtkNew<vtkIdTypeArray> ids;
    ids->SetNumberOfComponents(1);
    ids->InsertNextValue(cellId);

    vtkNew<vtkSelectionNode> selectionNode;
    selectionNode->SetFieldType(vtkSelectionNode::CELL);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(ids);

    vtkNew<vtkSelection> selection;
    selection->AddNode(selectionNode);

    vtkNew<vtkExtractSelection> extractSelection;
    extractSelection->SetInputConnection(0, sphereSource->GetOutputPort());
    extractSelection->SetInputData(1, selection);
    extractSelection->Update();

    mainCellMapper->SetInputConnection(extractSelection->GetOutputPort());
  }

  vtkNew<vtkActor> mainCellActor;
  mainCellActor->SetMapper(mainCellMapper);
  mainCellActor->GetProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());

  // Create a dataset with the neighbor cells
  {
    vtkNew<vtkIdTypeArray> ids;
    ids->SetNumberOfComponents(1);
    for (auto it1 = neighbors.begin(); it1 != neighbors.end(); ++it1)
    {
      ids->InsertNextValue(*it1);
    }

    vtkNew<vtkSelectionNode> selectionNode;
    selectionNode->SetFieldType(vtkSelectionNode::CELL);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(ids);

    vtkNew<vtkSelection> selection;
    selection->AddNode(selectionNode);

    vtkNew<vtkExtractSelection> extractSelection;
    extractSelection->SetInputConnection(0, sphereSource->GetOutputPort());
    extractSelection->SetInputData(1, selection);
    extractSelection->Update();

    neighborCellsMapper->SetInputConnection(extractSelection->GetOutputPort());
  }

  vtkNew<vtkActor> neighborCellsActor;
  neighborCellsActor->SetMapper(neighborCellsMapper);
  neighborCellsActor->GetProperty()->SetColor(
      colors->GetColor3d("Mint").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(sphereActor);
  renderer->AddActor(mainCellActor);
  renderer->AddActor(neighborCellsActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("CellEdgeNeighbors");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
