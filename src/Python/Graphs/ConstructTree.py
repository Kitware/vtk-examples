#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonDataModel import (
    vtkMutableDirectedGraph,
    vtkTree
)
from vtkmodules.vtkViewsInfovis import vtkGraphLayoutView


def main():
    graph = vtkMutableDirectedGraph()

    v1 = graph.AddVertex()
    v2 = graph.AddChild(v1)
    graph.AddChild(v1)
    graph.AddChild(v2)

    # equivalent to:
    # V1 = g.AddVertex()
    # V2 = g.AddVertex()
    # V3 = g.AddVertex()
    # V4 = g.AddVertex()

    # g.AddEdge ( V1, V2 )
    # g.AddEdge ( V1, V3 )
    # g.AddEdge ( V2, V4 )

    tree = vtkTree()
    success = tree.CheckedShallowCopy(graph)
    print('Success?', success)
    # std::cout << 'Success? ' << success << std::endl

    treeLayoutView = vtkGraphLayoutView()
    treeLayoutView.AddRepresentationFromInput(tree)
    treeLayoutView.SetLayoutStrategyToTree()
    treeLayoutView.ResetCamera()
    treeLayoutView.Render()
    treeLayoutView.GetInteractor().Start()


if __name__ == '__main__':
    main()
