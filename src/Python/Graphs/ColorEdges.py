#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkIntArray,
    vtkLookupTable
)
from vtkmodules.vtkCommonDataModel import vtkMutableDirectedGraph
from vtkmodules.vtkViewsCore import vtkViewTheme
from vtkmodules.vtkViewsInfovis import vtkGraphLayoutView


def main():
    colors = vtkNamedColors()

    graph = vtkMutableDirectedGraph()
    # Create a graph
    v1 = graph.AddVertex()
    v2 = graph.AddVertex()
    v3 = graph.AddVertex()

    graph.AddGraphEdge(v1, v2)
    graph.AddGraphEdge(v2, v3)

    # Create the color array
    edgeColors = vtkIntArray()
    edgeColors.SetNumberOfComponents(1)
    edgeColors.SetName('Color')

    lookupTable = vtkLookupTable()
    lookupTable.SetNumberOfTableValues(2)
    lookupTable.SetTableValue(0, colors.GetColor4d('Red'))
    lookupTable.SetTableValue(1, colors.GetColor4d('Lime'))
    lookupTable.Build()

    edgeColors.InsertNextValue(0)
    edgeColors.InsertNextValue(1)

    # Add the color array to the graph
    graph.GetEdgeData().AddArray(edgeColors)

    graphLayoutView = vtkGraphLayoutView()
    graphLayoutView.AddRepresentationFromInput(graph)
    graphLayoutView.SetLayoutStrategy('Simple 2D')
    graphLayoutView.GetLayoutStrategy().SetEdgeWeightField('Graphs')
    graphLayoutView.GetLayoutStrategy().SetWeightEdges(1)
    graphLayoutView.SetEdgeColorArrayName('Color')
    graphLayoutView.SetEdgeLabelVisibility(1)
    graphLayoutView.ColorEdgesOn()

    theme = vtkViewTheme()
    theme.SetCellLookupTable(lookupTable)

    graphLayoutView.ApplyViewTheme(theme)
    graphLayoutView.ResetCamera()
    graphLayoutView.GetRenderer().GetActiveCamera().Zoom(0.8)
    graphLayoutView.Render()
    graphLayoutView.GetLayoutStrategy().SetRandomSeed(0)
    graphLayoutView.GetInteractor().Initialize()
    graphLayoutView.GetInteractor().Start()


if __name__ == '__main__':
    main()
