#!/usr/bin/env python
# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonDataModel import vtkMutableDirectedGraph
from vtkmodules.vtkFiltersCore import vtkGlyph3D
from vtkmodules.vtkFiltersSources import (
    vtkGlyphSource2D,
    vtkGraphToPolyData
)
from vtkmodules.vtkInfovisLayout import (
    vtkGraphLayout,
    vtkSimple2DLayoutStrategy
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper
)
from vtkmodules.vtkViewsInfovis import vtkGraphLayoutView


def main():
    g = vtkMutableDirectedGraph()

    v1 = g.AddVertex()
    v2 = g.AddVertex()
    v3 = g.AddVertex()

    g.AddEdge(v1, v2)
    g.AddEdge(v2, v3)
    g.AddEdge(v3, v1)

    # Do layout manually before handing graph to the view.
    # This allows us to know the positions of edge arrows.
    graphLayoutView = vtkGraphLayoutView()

    layout = vtkGraphLayout()
    strategy = vtkSimple2DLayoutStrategy()
    layout.SetInputData(g)
    layout.SetLayoutStrategy(strategy)

    # Tell the view to use the vertex layout we provide
    graphLayoutView.SetLayoutStrategyToPassThrough()
    # The arrows will be positioned on a straight line between two
    # vertices so tell the view not to draw arcs for parallel edges
    graphLayoutView.SetEdgeLayoutStrategyToPassThrough()

    # Add the graph to the view. This will render vertices and edges,
    # but not edge arrows.
    graphLayoutView.AddRepresentationFromInputConnection(layout.GetOutputPort())

    # Manually create an actor containing the glyphed arrows.
    graphToPoly = vtkGraphToPolyData()
    graphToPoly.SetInputConnection(layout.GetOutputPort())
    graphToPoly.EdgeGlyphOutputOn()

    # Set the position (0: edge start, 1: edge end) where
    # the edge arrows should go.
    graphToPoly.SetEdgeGlyphPosition(0.98)

    # Make a simple edge arrow for glyphing.
    arrowSource = vtkGlyphSource2D()
    arrowSource.SetGlyphTypeToEdgeArrow()
    arrowSource.SetScale(0.1)
    arrowSource.Update()

    # Use Glyph3D to repeat the glyph on all edges.
    arrowGlyph = vtkGlyph3D()
    arrowGlyph.SetInputConnection(0, graphToPoly.GetOutputPort(1))
    arrowGlyph.SetInputConnection(1, arrowSource.GetOutputPort())

    # Add the edge arrow actor to the view.
    arrowMapper = vtkPolyDataMapper()
    arrowMapper.SetInputConnection(arrowGlyph.GetOutputPort())
    arrowActor = vtkActor()
    arrowActor.SetMapper(arrowMapper)
    graphLayoutView.GetRenderer().AddActor(arrowActor)

    graphLayoutView.ResetCamera()
    graphLayoutView.Render()
    graphLayoutView.GetInteractor().Start()


if __name__ == '__main__':
    main()
