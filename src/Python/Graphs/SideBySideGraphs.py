#!/usr/bin/env python
import vtk


def main():
    colors = vtk.vtkNamedColors()

    # Create the first graph
    g0 = vtk.vtkMutableUndirectedGraph()

    v1 = g0.AddVertex()
    v2 = g0.AddVertex()
    v3 = g0.AddVertex()

    g0.AddEdge(v1, v2)
    g0.AddEdge(v2, v3)
    g0.AddEdge(v1, v3)

    # Create points
    points = vtk.vtkPoints()
    points.InsertNextPoint(0.0, 0.0, 0.0)
    points.InsertNextPoint(1.0, 0.0, 0.0)
    points.InsertNextPoint(0.0, 1.0, 0.0)

    # Add the coordinates of the points to the graph
    g0.SetPoints(points)

    # Create the second graph
    g1 = vtk.vtkMutableUndirectedGraph()

    v1 = g1.AddVertex()
    v2 = g1.AddVertex()

    g1.AddEdge(v1, v2)

    # Create points
    points = vtk.vtkPoints()
    points.InsertNextPoint(0.0, 0.0, 0.0)
    points.InsertNextPoint(1.0, 0.0, 0.0)

    # Add the coordinates of the points to the graph
    g1.SetPoints(points)

    # There will be one render window
    ren_win = vtk.vtkRenderWindow()
    ren_win.SetSize(600, 300)
    ren_win.SetWindowName('SideBySideGraphs')

    iren = vtk.vtkRenderWindowInteractor()

    # Define viewport ranges
    # (xmin, ymin, xmax, ymax)
    left_viewport = [0.0, 0.0, 0.5, 1.0]
    right_viewport = [0.5, 0.0, 1.0, 1.0]

    graph_layout_view0 = vtk.vtkGraphLayoutView()
    graph_layout_view0.SetRenderWindow(ren_win)
    graph_layout_view0.SetInteractor(iren)
    graph_layout_view0.GetRenderer().SetViewport(left_viewport)
    graph_layout_view0.AddRepresentationFromInput(g0)
    # If we create a layout object directly, just set the pointer through this method.
    # graph_layout_view0.SetLayoutStrategy(force_directed)
    graph_layout_view0.SetLayoutStrategyToForceDirected()
    graph_layout_view0.GetRenderer().SetBackground(colors.GetColor3d('Navy'))
    graph_layout_view0.GetRenderer().SetBackground2(colors.GetColor3d('MidnightBlue'))
    graph_layout_view0.Render()
    graph_layout_view0.ResetCamera()

    graph_layout_view1 = vtk.vtkGraphLayoutView()
    graph_layout_view1.SetRenderWindow(ren_win)
    graph_layout_view1.SetInteractor(iren)
    graph_layout_view1.GetRenderer().SetViewport(right_viewport)
    graph_layout_view1.AddRepresentationFromInput(g0)
    # If we create a layout object directly, just set the pointer through this method.
    # graph_layout_view1.SetLayoutStrategy(force_directed)
    graph_layout_view1.SetLayoutStrategyToForceDirected()
    graph_layout_view1.AddRepresentationFromInput(g1)
    graph_layout_view1.GetRenderer().SetBackground(colors.GetColor3d('DarkGreen'))
    graph_layout_view1.GetRenderer().SetBackground2(colors.GetColor3d('ForestGreen'))
    graph_layout_view1.Render()
    graph_layout_view1.ResetCamera()

    # graph_layout_view0.GetInteractor().Start()
    iren.Start()


if __name__ == '__main__':
    main()
