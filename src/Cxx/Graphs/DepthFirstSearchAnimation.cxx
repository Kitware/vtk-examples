#include <vtkCommand.h>
#include <vtkDataSetAttributes.h>
#include <vtkGraphLayoutView.h>
#include <vtkIntArray.h>
#include <vtkLookupTable.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkTree.h>
#include <vtkTreeDFSIterator.h>
#include <vtkUnsignedCharArray.h>
#include <vtkViewTheme.h>

class vtkTimerCallback : public vtkCommand
{
public:
  static vtkTimerCallback* New()
  {
    vtkTimerCallback* cb = new vtkTimerCallback;
    return cb;
  }
  virtual void Execute(vtkObject* vtkNotUsed(caller), unsigned long eventId,
                       void* vtkNotUsed(callData))
  {
    if (eventId != vtkCommand::TimerEvent)
    {
      return;
    }

    if (this->dfs->HasNext())
    {
      vtkIdType nextVertex = this->dfs->Next();
      std::cout << "Next vertex: " << nextVertex << std::endl;
      dynamic_cast<vtkIntArray*>(this->Tree->GetVertexData()->GetArray("color"))
          ->SetValue(nextVertex, 10);
      this->Tree->Modified();
      this->GraphLayoutView->AddRepresentationFromInput(this->Tree);
      this->GraphLayoutView->Render();
    }
  }

  void SetDFS(vtkTreeDFSIterator* dfs)
  {
    this->dfs = dfs;
  }
  void SetTree(vtkTree* tree)
  {
    this->Tree = tree;
  }
  void SetGraphLayoutView(vtkGraphLayoutView* view)
  {
    this->GraphLayoutView = view;
  }

private:
  vtkTreeDFSIterator* dfs = nullptr;
  vtkTree* Tree = nullptr;
  vtkGraphLayoutView* GraphLayoutView = nullptr;
};

int main(int, char*[])
{
  vtkNew<vtkMutableDirectedGraph> graph;

  // Create a tree
  vtkIdType v1 = graph->AddVertex();
  vtkIdType v2 = graph->AddChild(v1);
  graph->AddChild(v1);
  graph->AddChild(v2);

  vtkNew<vtkTree> tree;
  tree->CheckedShallowCopy(graph);

  // Create the color array
  vtkNew<vtkIntArray> vertexColors;
  vertexColors->SetNumberOfComponents(1);
  vertexColors->SetName("color");

  vtkNew<vtkLookupTable> lookupTable;

  lookupTable->SetTableRange(0.0, 10.0);
  lookupTable->Build();

  for (vtkIdType i = 0; i < tree->GetNumberOfVertices(); i++)
  {
    vertexColors->InsertNextValue(0);
  }

  // Add the color array to the tree
  tree->GetVertexData()->AddArray(vertexColors);

  // Create a depth first search iterator
  vtkNew<vtkTreeDFSIterator> dfs;
  vtkIdType root = tree->GetRoot();
  dfs->SetStartVertex(root);
  dfs->SetTree(tree);

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(tree);
  graphLayoutView->SetLayoutStrategyToTree();

  graphLayoutView->SetVertexColorArrayName("color");
  graphLayoutView->ColorVerticesOn();

  vtkNew<vtkViewTheme> theme;
  theme->SetPointLookupTable(lookupTable);
  theme->ScalePointLookupTableOff();

  graphLayoutView->ApplyViewTheme(theme);
  graphLayoutView->ResetCamera();
  graphLayoutView->Render();

  // Sign up to receive TimerEvent
  vtkNew<vtkTimerCallback> cb;
  cb->SetDFS(dfs);
  cb->SetTree(tree);
  cb->SetGraphLayoutView(graphLayoutView);

  graphLayoutView->GetInteractor()->CreateRepeatingTimer(1000);
  graphLayoutView->GetInteractor()->AddObserver(vtkCommand::TimerEvent, cb);

  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
