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

    layoutView = vtk.vtkGraphLayoutView()
    layoutView.AddRepresentationFromInput(g)
    layoutView.ApplyViewTheme(theme)
    layoutView.ScaledGlyphsOn()
    layoutView.SetScalingArrayName('Scales')
    layoutView.SetVertexColorArrayName('Color')
    layoutView.ColorVerticesOn()
    rGraph = vtk.vtkRenderedGraphRepresentation()
    gGlyph = vtk.vtkGraphToGlyphs()
    rGraph.SafeDownCast(layoutView.GetRepresentation()).SetGlyphType(gGlyph.CIRCLE)
    layoutView.ResetCamera()
    layoutView.Render()
    layoutView.GetInteractor().Start()


if __name__ == '__main__':
    main()
