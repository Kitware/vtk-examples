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

  // Find all cells connected to point 0
  vtkIdType cellId = 0;

  vtkNew<vtkIdList> cellPointIds;
  triangleFilter->GetOutput()->GetCellPoints(cellId, cellPointIds);

  // neighbor cells may be listed multiple times
  // use std::set instead of std::list if you want a unique list of neighbors
  std::list<vtkIdType> neighbors;

  /* For each vertex of the cell, we calculate which cells use that point.
   So if we do this for each vertex, we have all the neighbors.
   In the case of using "cellPointIds" as a parameter of "GetCellNeighbors",
   we will obtain an empty set. This is because the only cell that is using that
   set of points is the current one. That is why we have to make each vertice at
   time.*/

  for (vtkIdType i = 0; i < cellPointIds->GetNumberOfIds(); i++)
  {
    vtkNew<vtkIdList> idList;
    idList->InsertNextId(cellPointIds->GetId(i));

    // get the neighbors of the cell
    vtkNew<vtkIdList> neighborCellIds;

    triangleFilter->GetOutput()->GetCellNeighbors(cellId, idList,
                                                  neighborCellIds);

    for (vtkIdType j = 0; j < neighborCellIds->GetNumberOfIds(); j++)
    {
      neighbors.push_back(neighborCellIds->GetId(j));
    }
  }

  std::cout << "Point neighbor ids are: " << std::endl;

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
  sphereActor->GetProperty()->SetLineWidth(3);

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
  renderWindow->SetWindowName("CellPointNeighbors");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(sphereActor);
  renderer->AddActor(mainCellActor);
  renderer->AddActor(neighborCellsActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->SetSize(640, 480);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
