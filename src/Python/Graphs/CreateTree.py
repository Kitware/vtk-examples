#!/usr/bin/env python

# This example creates a tree and labels the vertices and edges.

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonCore import vtkStringArray
from vtkmodules.vtkCommonDataModel import (
    vtkMutableDirectedGraph,
    vtkTree
)
from vtkmodules.vtkViewsCore import vtkViewTheme
from vtkmodules.vtkViewsInfovis import vtkGraphLayoutView


def main():
    graph = vtkMutableDirectedGraph()

    a = graph.AddVertex()
    b = graph.AddChild(a)
    c = graph.AddChild(a)
    d = graph.AddChild(b)
    e = graph.AddChild(c)
    f = graph.AddChild(c)

    vertex_labels = vtkStringArray()
    vertex_labels.SetName('VertexLabel')
    vertex_labels.InsertValue(a, 'a')
    vertex_labels.InsertValue(b, 'b')
    vertex_labels.InsertValue(c, 'c')
    vertex_labels.InsertValue(d, 'd')
    vertex_labels.InsertValue(e, 'e')
    vertex_labels.InsertValue(f, 'f')
    graph.GetVertexData().AddArray(vertex_labels)
    edge_labels = vtkStringArray()
    edge_labels.SetName('EdgeLabel')
    edge_labels.InsertValue(graph.GetEdgeId(a, b), 'a -> b')
    edge_labels.InsertValue(graph.GetEdgeId(a, c), 'a -> c')
    edge_labels.InsertValue(graph.GetEdgeId(b, d), 'b -> d')
    edge_labels.InsertValue(graph.GetEdgeId(c, e), 'c -> e')
    edge_labels.InsertValue(graph.GetEdgeId(c, f), 'c -> f')
    graph.GetEdgeData().AddArray(edge_labels)

    tree = vtkTree()
    valid_tree = tree.CheckedShallowCopy(graph)
    if not valid_tree:
        print('Invalid tree')
        return

    view = vtkGraphLayoutView()
    view.SetRepresentationFromInput(tree)
    # Apply a theme to the views
    theme = vtkViewTheme()
    view.ApplyViewTheme(theme.CreateMellowTheme())
    view.SetVertexColorArrayName('VertexDegree')
    view.SetColorVertices(True)
    view.SetVertexLabelArrayName('VertexLabel')
    view.SetVertexLabelVisibility(True)
    view.SetEdgeLabelArrayName('EdgeLabel')
    view.SetEdgeLabelVisibility(True)
    view.SetLayoutStrategyToTree()

    view.ResetCamera()
    view.GetRenderWindow().SetSize(600, 600)
    view.GetRenderWindow().SetWindowName('CreateTree')
    view.GetRenderWindow().Render()
    view.GetInteractor().Initialize()
    view.GetInteractor().Start()


if __name__ == '__main__':
    main()
