#!/usr/bin/env python

# This example creates a tree and labels the vertices and edges.

import vtk


def main():
    graph = vtk.vtkMutableDirectedGraph()

    a = graph.AddVertex()
    b = graph.AddChild(a)
    c = graph.AddChild(a)
    d = graph.AddChild(b)
    e = graph.AddChild(c)
    f = graph.AddChild(c)

    vertex_labels = vtk.vtkStringArray()
    vertex_labels.SetName('VertexLabel')
    vertex_labels.InsertValue(a, 'a')
    vertex_labels.InsertValue(b, 'b')
    vertex_labels.InsertValue(c, 'c')
    vertex_labels.InsertValue(d, 'd')
    vertex_labels.InsertValue(e, 'e')
    vertex_labels.InsertValue(f, 'f')
    graph.GetVertexData().AddArray(vertex_labels)
    edge_labels = vtk.vtkStringArray()
    edge_labels.SetName('EdgeLabel')
    edge_labels.InsertValue(graph.GetEdgeId(a, b), 'a -> b')
    edge_labels.InsertValue(graph.GetEdgeId(a, c), 'a -> c')
    edge_labels.InsertValue(graph.GetEdgeId(b, d), 'b -> d')
    edge_labels.InsertValue(graph.GetEdgeId(c, e), 'c -> e')
    edge_labels.InsertValue(graph.GetEdgeId(c, f), 'c -> f')
    graph.GetEdgeData().AddArray(edge_labels)

    tree = vtk.vtkTree()
    valid_tree = tree.CheckedShallowCopy(graph)
    if not valid_tree:
        print('Invalid tree')
        return

    view = vtk.vtkGraphLayoutView()
    view.SetRepresentationFromInput(tree)
    # Apply a theme to the views
    theme = vtk.vtkViewTheme()
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
