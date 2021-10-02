#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkIntArray,
    vtkLookupTable,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import vtkMutableDirectedGraph
from vtkmodules.vtkViewsCore import vtkViewTheme
from vtkmodules.vtkViewsInfovis import vtkGraphLayoutView


def main():
    colors = vtkNamedColors()

    # Create a graph
    graph = vtkMutableDirectedGraph()

    v1 = graph.AddVertex()
    v2 = graph.AddVertex()
    v3 = graph.AddVertex()
    graph.AddEdge(v1, v2)
    graph.AddEdge(v2, v3)

    # Manually set the position of the vertices
    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(1, 0, 0)
    points.InsertNextPoint(2, 0, 0)

    graph.SetPoints(points)

    # Create the color array
    vertexColors = vtkIntArray()
    vertexColors.SetNumberOfComponents(1)
    vertexColors.SetName('Color')

    lookupTable = vtkLookupTable()
    lookupTable.SetNumberOfTableValues(3)
    lookupTable.SetTableValue(0, colors.GetColor4d('Red'))
    lookupTable.SetTableValue(1, colors.GetColor4d('White'))
    lookupTable.SetTableValue(2, colors.GetColor4d('Lime'))
    lookupTable.Build()

    vertexColors.InsertNextValue(0)
    vertexColors.InsertNextValue(1)
    vertexColors.InsertNextValue(2)

    # Add the color array to the graph
    graph.GetVertexData().AddArray(vertexColors)

    # Visualize
    graphLayoutView = vtkGraphLayoutView()
    graphLayoutView.AddRepresentationFromInput(graph)
    graphLayoutView.SetLayoutStrategyToPassThrough()
    graphLayoutView.SetVertexColorArrayName('Color')
    graphLayoutView.ColorVerticesOn()

    theme = vtkViewTheme()
    theme.SetPointLookupTable(lookupTable)

    graphLayoutView.ApplyViewTheme(theme)
    graphLayoutView.ResetCamera()
    graphLayoutView.GetInteractor().Initialize()
    graphLayoutView.GetRenderer().GetActiveCamera().Zoom(0.8)
    graphLayoutView.GetInteractor().Start()


if __name__ == '__main__':
    main()
