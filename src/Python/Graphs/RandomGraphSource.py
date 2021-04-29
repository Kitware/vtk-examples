#!/usr/bin/env python
import vtk


def main():
    colors = vtk.vtkNamedColors()

    random_graph_source = vtk.vtkRandomGraphSource()
    random_graph_source.SetNumberOfVertices(5)
    random_graph_source.SetNumberOfEdges(4)
    # This ensures repeatable results for testing. Turn this off for real use.
    random_graph_source.SetSeed(123)
    random_graph_source.Update()

    force_directed = vtk.vtkForceDirectedLayoutStrategy()

    graph_layout_view = vtk.vtkGraphLayoutView()
    graph_layout_view.AddRepresentationFromInput(random_graph_source.GetOutput())
    # If we create a layout object directly, just set the pointer through this method.
    # graph_layout_view.SetLayoutStrategy(force_directed)
    graph_layout_view.SetLayoutStrategyToForceDirected()
    graph_layout_view.GetRenderer().SetBackground(colors.GetColor3d('Navy'))
    graph_layout_view.GetRenderer().SetBackground2(colors.GetColor3d('MidnightBlue'))
    graph_layout_view.GetRenderWindow().SetWindowName('RandomGraphSource')
    graph_layout_view.Render()
    graph_layout_view.ResetCamera()
    graph_layout_view.GetInteractor().Start()


if __name__ == '__main__':
    main()
