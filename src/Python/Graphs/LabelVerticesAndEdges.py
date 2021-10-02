#!/usr/bin/env python
# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkDoubleArray,
    vtkIntArray
)
from vtkmodules.vtkCommonDataModel import vtkMutableUndirectedGraph
from vtkmodules.vtkInfovisLayout import vtkCircularLayoutStrategy
from vtkmodules.vtkViewsInfovis import vtkGraphLayoutView


def main():
    colors = vtkNamedColors()

    g = vtkMutableUndirectedGraph()

    # Create 3 vertices
    v1 = g.AddVertex()
    v2 = g.AddVertex()
    v3 = g.AddVertex()

    # Create a fully connected graph
    g.AddEdge(v1, v2)
    g.AddEdge(v2, v3)
    g.AddEdge(v1, v3)

    # Create the edge weight array
    weights = vtkDoubleArray()
    weights.SetNumberOfComponents(1)
    weights.SetName('Weights')

    # Set the edge weights
    weights.InsertNextValue(1.0)
    weights.InsertNextValue(1.0)
    weights.InsertNextValue(2.0)

    # Create an array for the vertex labels
    vertexIDs = vtkIntArray()
    vertexIDs.SetNumberOfComponents(1)
    vertexIDs.SetName('VertexIDs')

    # Set the vertex labels
    vertexIDs.InsertNextValue(0)
    vertexIDs.InsertNextValue(1)
    vertexIDs.InsertNextValue(2)

    # Add the edge weight array to the graph
    g.GetEdgeData().AddArray(weights)
    g.GetVertexData().AddArray(vertexIDs)

    circularLayoutStrategy = vtkCircularLayoutStrategy()

    graphLayoutView = vtkGraphLayoutView()
    graphLayoutView.AddRepresentationFromInput(g)

    graphLayoutView.SetLayoutStrategy(circularLayoutStrategy)
    graphLayoutView.SetVertexLabelVisibility(1)
    graphLayoutView.SetEdgeLabelVisibility(1)
    graphLayoutView.SetEdgeLabelArrayName('Weights')  # default is 'labels'
    graphLayoutView.SetVertexLabelArrayName('VertexIDs')  # default is 'labels'
    graphLayoutView.GetRepresentation().GetVertexLabelTextProperty().SetColor(
        colors.GetColor3d('Yellow'))
    graphLayoutView.GetRepresentation().GetEdgeLabelTextProperty().SetColor(
        colors.GetColor3d('Lime'))
    graphLayoutView.ResetCamera()
    graphLayoutView.Render()
    graphLayoutView.GetInteractor().Start()


if __name__ == '__main__':
    main()
