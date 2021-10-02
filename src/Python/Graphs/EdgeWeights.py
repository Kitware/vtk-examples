#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkDoubleArray
from vtkmodules.vtkCommonDataModel import vtkMutableDirectedGraph
from vtkmodules.vtkInfovisLayout import vtkForceDirectedLayoutStrategy
from vtkmodules.vtkViewsInfovis import vtkGraphLayoutView


def main():
    colors = vtkNamedColors()

    g = vtkMutableDirectedGraph()

    # Create 3 vertices
    v1 = g.AddVertex()
    v2 = g.AddVertex()
    v3 = g.AddVertex()

    # Create a fully connected graph
    g.AddGraphEdge(v1, v2)
    g.AddGraphEdge(v2, v3)
    g.AddGraphEdge(v1, v3)

    # Create the edge weight array
    weights = vtkDoubleArray()
    weights.SetNumberOfComponents(1)
    weights.SetName('Weights')

    # Set the edge weights
    weights.InsertNextValue(1.0)
    weights.InsertNextValue(1.0)
    weights.InsertNextValue(2.0)

    # Add the edge weight array to the graph
    g.GetEdgeData().AddArray(weights)

    print('Number of edges:', g.GetNumberOfEdges())

    force_directed = vtkForceDirectedLayoutStrategy()

    graph_layout_view = vtkGraphLayoutView()
    graph_layout_view.AddRepresentationFromInput(g)
    # If we create a layout object directly, just set the pointer through this method.
    # graph_layout_view.SetLayoutStrategy(force_directed)
    graph_layout_view.SetLayoutStrategyToForceDirected()
    graph_layout_view.AddRepresentationFromInput(g)
    graph_layout_view.GetLayoutStrategy().SetEdgeWeightField('Weights')
    graph_layout_view.GetLayoutStrategy().SetWeightEdges(1)
    graph_layout_view.SetEdgeLabelArrayName('Weights')
    graph_layout_view.EdgeLabelVisibilityOn()
    graph_layout_view.GetRenderer().SetBackground(colors.GetColor3d('Navy'))
    graph_layout_view.GetRenderer().SetBackground2(colors.GetColor3d('MidnightBlue'))
    graph_layout_view.GetRenderWindow().SetWindowName('EdgeWeights')
    graph_layout_view.ResetCamera()
    graph_layout_view.Render()
    graph_layout_view.GetInteractor().Start()


if __name__ == '__main__':
    main()
