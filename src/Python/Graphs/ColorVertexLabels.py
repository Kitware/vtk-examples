#!/usr/bin/env python

import vtk


def main():
    colors = vtk.vtkNamedColors()

    # Create a graph
    graph = vtk.vtkMutableDirectedGraph()

    v1 = graph.AddVertex()
    v2 = graph.AddVertex()
    graph.AddEdge(v1, v2)

    # Create an array for the vertex labels
    vertexIDs = vtk.vtkIntArray()
    vertexIDs.SetNumberOfComponents(1)
    vertexIDs.SetName('VertexIDs')

    # Set the vertex labels
    vertexIDs.InsertNextValue(0)
    vertexIDs.InsertNextValue(1)

    # Add the array to the graph
    graph.GetVertexData().AddArray(vertexIDs)

    graphLayoutView = vtk.vtkGraphLayoutView()
    graphLayoutView.AddRepresentationFromInput(graph)
    graphLayoutView.SetVertexLabelVisibility(1)

    rGraph = vtk.vtkRenderedGraphRepresentation()
    rGraph.SafeDownCast(graphLayoutView.GetRepresentation()).GetVertexLabelTextProperty().SetColor(
        colors.GetColor3d('Red'))
    graphLayoutView.SetLayoutStrategyToSimple2D()
    graphLayoutView.SetVertexLabelArrayName('VertexIDs')
    graphLayoutView.SetVertexLabelVisibility(True)
    graphLayoutView.ResetCamera()
    graphLayoutView.GetRenderer().GetActiveCamera().Zoom(0.8)
    graphLayoutView.Render()
    graphLayoutView.GetInteractor().Start()


if __name__ == '__main__':
    main()
