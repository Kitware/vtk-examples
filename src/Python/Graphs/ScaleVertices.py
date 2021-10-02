#!/usr/bin/env python
# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkFloatArray,
    vtkIntArray,
    vtkLookupTable
)
from vtkmodules.vtkCommonDataModel import vtkMutableUndirectedGraph
from vtkmodules.vtkInfovisLayout import vtkForceDirectedLayoutStrategy
from vtkmodules.vtkRenderingCore import vtkGraphToGlyphs
from vtkmodules.vtkViewsCore import vtkViewTheme
from vtkmodules.vtkViewsInfovis import (
    vtkGraphLayoutView,
    vtkRenderedGraphRepresentation
)


def main():
    colors = vtkNamedColors()

    g = vtkMutableUndirectedGraph()

    v1 = g.AddVertex()
    v2 = g.AddVertex()

    g.AddEdge(v1, v2)
    g.AddEdge(v1, v2)

    scales = vtkFloatArray()
    scales.SetNumberOfComponents(1)
    scales.SetName('Scales')
    scales.InsertNextValue(2.0)
    scales.InsertNextValue(5.0)

    # Add the scale array to the graph
    g.GetVertexData().AddArray(scales)

    # Create the color array
    vertexColors = vtkIntArray()
    vertexColors.SetNumberOfComponents(1)
    vertexColors.SetName('Color')

    lookupTable = vtkLookupTable()
    lookupTable.SetNumberOfTableValues(2)
    lookupTable.SetTableValue(0, colors.GetColor4d('Yellow'))
    lookupTable.SetTableValue(1, colors.GetColor4d('Lime'))
    lookupTable.Build()

    vertexColors.InsertNextValue(0)
    vertexColors.InsertNextValue(1)

    # Add the color array to the graph
    g.GetVertexData().AddArray(vertexColors)

    theme = vtkViewTheme()
    theme.SetPointLookupTable(lookupTable)

    force_directed = vtkForceDirectedLayoutStrategy()

    layout_view = vtkGraphLayoutView()
    # If we create a layout object directly, just set the pointer through this method.
    # graph_layout_view.SetLayoutStrategy(force_directed)
    layout_view.SetLayoutStrategyToForceDirected()
    layout_view.AddRepresentationFromInput(g)
    layout_view.ApplyViewTheme(theme)
    layout_view.ScaledGlyphsOn()
    layout_view.SetScalingArrayName('Scales')
    layout_view.SetVertexColorArrayName('Color')
    layout_view.ColorVerticesOn()
    rGraph = vtkRenderedGraphRepresentation()
    gGlyph = vtkGraphToGlyphs()
    rGraph.SafeDownCast(layout_view.GetRepresentation()).SetGlyphType(gGlyph.CIRCLE)
    layout_view.GetRenderer().SetBackground(colors.GetColor3d('Navy'))
    layout_view.GetRenderer().SetBackground2(colors.GetColor3d('MidnightBlue'))
    layout_view.GetRenderWindow().SetWindowName('ScaleVertices')
    layout_view.Render()
    layout_view.ResetCamera()
    layout_view.GetInteractor().Start()


if __name__ == '__main__':
    main()
