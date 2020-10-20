#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkExtractEdges.h>
#include <vtkExtractSelection.h>
#include <vtkIdList.h>
#include <vtkIdTypeArray.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTriangleFilter.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>

namespace {

vtkSmartPointer<vtkIdList> GetConnectedVertices(vtkPolyData* mesh, int id);

}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInputConnection(sphereSource->GetOutputPort());
  triangleFilter->Update();

  vtkNew<vtkExtractEdges> extractEdges;
  extractEdges->SetInputConnection(triangleFilter->GetOutputPort());
  extractEdges->Update();

  vtkSmartPointer<vtkPolyData> mesh = extractEdges->GetOutput();

  vtkSmartPointer<vtkIdList> connectedVertices = GetConnectedVertices(mesh, 0);

  vtkNew<vtkIdTypeArray> ids;
  ids->SetNumberOfComponents(1);

  std::cout << "Connected vertices: ";
  for (vtkIdType i = 0; i < connectedVertices->GetNumberOfIds(); i++)
  {
    std::cout << connectedVertices->GetId(i) << " ";
    ids->InsertNextValue(connectedVertices->GetId(i));
  }
  std::cout << std::endl;

  vtkNew<vtkDataSetMapper> connectedVertexMapper;

  {
    vtkNew<vtkSelectionNode> selectionNode;
    selectionNode->SetFieldType(vtkSelectionNode::POINT);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(ids);

    vtkNew<vtkSelection> selection;
    selection->AddNode(selectionNode);

    vtkNew<vtkExtractSelection> extractSelection;

    extractSelection->SetInputConnection(0, extractEdges->GetOutputPort());
    extractSelection->SetInputData(1, selection);
    extractSelection->Update();

    vtkNew<vtkVertexGlyphFilter> glyphFilter;
    glyphFilter->SetInputConnection(extractSelection->GetOutputPort());
    glyphFilter->Update();

    connectedVertexMapper->SetInputConnection(glyphFilter->GetOutputPort());
  }

  vtkNew<vtkActor> connectedVertexActor;
  connectedVertexActor->SetMapper(connectedVertexMapper);
  connectedVertexActor->GetProperty()->SetColor(
      colors->GetColor3d("Red").GetData());
  connectedVertexActor->GetProperty()->SetPointSize(5);

  vtkNew<vtkDataSetMapper> queryVertexMapper;

  {
    vtkNew<vtkIdTypeArray> ids2;
    ids2->SetNumberOfComponents(1);
    ids2->InsertNextValue(0);

    vtkNew<vtkSelectionNode> selectionNode;
    selectionNode->SetFieldType(vtkSelectionNode::POINT);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(ids2);

    vtkNew<vtkSelection> selection;
    selection->AddNode(selectionNode);

    vtkNew<vtkExtractSelection> extractSelection;

    extractSelection->SetInputConnection(0, extractEdges->GetOutputPort());
    extractSelection->SetInputData(1, selection);
    extractSelection->Update();

    vtkNew<vtkVertexGlyphFilter> glyphFilter;
    glyphFilter->SetInputConnection(extractSelection->GetOutputPort());
    glyphFilter->Update();

    queryVertexMapper->SetInputConnection(glyphFilter->GetOutputPort());
  }

  vtkNew<vtkActor> queryVertexActor;
  queryVertexActor->SetMapper(queryVertexMapper);
  queryVertexActor->GetProperty()->SetColor(
      colors->GetColor3d("Lime").GetData());
  queryVertexActor->GetProperty()->SetPointSize(5);

  vtkNew<vtkDataSetMapper> sphereMapper;
  sphereMapper->SetInputConnection(extractEdges->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(colors->GetColor3d("Snow").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("VertexConnectivity");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(sphereActor);
  renderer->AddActor(queryVertexActor);
  renderer->AddActor(connectedVertexActor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

vtkSmartPointer<vtkIdList> GetConnectedVertices(vtkPolyData* mesh, int id)
{
  vtkNew<vtkIdList> connectedVertices;

  // get all cells that vertex 'id' is a part of
  vtkNew<vtkIdList> cellIdList;
  mesh->GetPointCells(id, cellIdList);

  /*
  cout << "Vertex 0 is used in cells ";
  for(vtkIdType i = 0; i < cellIdList->GetNumberOfIds(); i++)
    {
    cout << cellIdList->GetId(i) << ", ";
    }
  cout << endl;
  */

  for (vtkIdType i = 0; i < cellIdList->GetNumberOfIds(); i++)
  {
    // cout << "id " << i << " : " << cellIdList->GetId(i) << endl;

    vtkNew<vtkIdList> pointIdList;
    mesh->GetCellPoints(cellIdList->GetId(i), pointIdList);

    // cout << "End points are " << pointIdList->GetId(0) << " and " <<
    // pointIdList->GetId(1) << endl;

    if (pointIdList->GetId(0) != id)
    {
      // cout << "Connected to " << pointIdList->GetId(0) << endl;
      connectedVertices->InsertNextId(pointIdList->GetId(0));
    }
    else
    {
      // cout << "Connected to " << pointIdList->GetId(1) << endl;
      connectedVertices->InsertNextId(pointIdList->GetId(1));
    }
  }

  return connectedVertices;
}

} // namespace
