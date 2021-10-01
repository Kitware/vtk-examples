#!/usr/bin/env python

import vtkmodules.all as vtk


def main():
    colors = vtk.vtkNamedColors()

    g = vtk.vtkMutableUndirectedGraph()

    v1 = g.AddVertex()
    v2 = g.AddVertex()

    g.AddEdge(v1, v2)
    print('Number of vertices:', g.GetNumberOfVertices())
    print('Number of edges:', g.GetNumberOfEdges())

    g.AddEdge(v1, v2)
    print('Number of vertices:', g.GetNumberOfVertices())
    print('Number of edges:', g.GetNumberOfEdges())

    force_directed = vtk.vtkForceDirectedLayoutStrategy()

    graph_layout_view = vtk.vtkGraphLayoutView()
    graph_layout_view.AddRepresentationFromInput(g)
    # If we create a layout object directly, just set the pointer through this method.
    # graph_layout_view.SetLayoutStrategy(force_directed)
    graph_layout_view.SetLayoutStrategyToForceDirected()
    graph_layout_view.ResetCamera()
    graph_layout_view.GetRenderer().SetBackground(colors.GetColor3d('Navy'))
    graph_layout_view.GetRenderer().SetBackground2(colors.GetColor3d('MidnightBlue'))
    graph_layout_view.GetRenderWindow().SetWindowName('ConstructGraph')
    graph_layout_view.Render()
    graph_layout_view.GetInteractor().Start()


if __name__ == '__main__':
    main()
