#!/usr/bin/env python
import vtk


def main():
    colors = vtk.vtkNamedColors()

    g = vtk.vtkMutableUndirectedGraph()

    v1 = g.AddVertex()
    v2 = g.AddVertex()

    g.AddEdge(v1, v2)
    g.AddEdge(v1, v2)

    scales = vtk.vtkFloatArray()
    scales.SetNumberOfComponents(1)
    scales.SetName('Scales')
    scales.InsertNextValue(2.0)
    scales.InsertNextValue(5.0)

    # Add the scale array to the graph
    g.GetVertexData().AddArray(scales)

    # Create the color array
    vertexColors = vtk.vtkIntArray()
    vertexColors.SetNumberOfComponents(1)
    vertexColors.SetName('Color')

    lookupTable = vtk.vtkLookupTable()
    lookupTable.SetNumberOfTableValues(2)
    lookupTable.SetTableValue(0, colors.GetColor4d('Yellow'))
    lookupTable.SetTableValue(1, colors.GetColor4d('Lime'))
    lookupTable.Build()

    vertexColors.InsertNextValue(0)
    vertexColors.InsertNextValue(1)

    # Add the color array to the graph
    g.GetVertexData().AddArray(vertexColors)

    theme = vtk.vtkViewTheme()
    theme.SetPointLookupTable(lookupTable)

    force_directed = vtk.vtkForceDirectedLayoutStrategy()

    layout_view = vtk.vtkGraphLayoutView()
    # If we create a layout object directly, just set the pointer through this method.
    # graph_layout_view.SetLayoutStrategy(force_directed)
    layout_view.SetLayoutStrategyToForceDirected()
    layout_view.AddRepresentationFromInput(g)
    layout_view.ApplyViewTheme(theme)
    layout_view.ScaledGlyphsOn()
    layout_view.SetScalingArrayName('Scales')
    layout_view.SetVertexColorArrayName('Color')
    layout_view.ColorVerticesOn()
    rGraph = vtk.vtkRenderedGraphRepresentation()
    gGlyph = vtk.vtkGraphToGlyphs()
    rGraph.SafeDownCast(layout_view.GetRepresentation()).SetGlyphType(gGlyph.CIRCLE)
    layout_view.GetRenderer().SetBackground(colors.GetColor3d('Navy'))
    layout_view.GetRenderer().SetBackground2(colors.GetColor3d('MidnightBlue'))
    layout_view.GetRenderWindow().SetWindowName('ScaleVertices')
    layout_view.Render()
    layout_view.ResetCamera()
    layout_view.GetInteractor().Start()


if __name__ == '__main__':
    main()
