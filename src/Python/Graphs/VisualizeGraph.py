#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonDataModel import vtkMutableDirectedGraph
from vtkmodules.vtkViewsInfovis import vtkGraphLayoutView


def main():
    g = vtkMutableDirectedGraph()

    v1 = g.AddVertex()
    v2 = g.AddVertex()

    g.AddGraphEdge(v1, v2)
    g.AddGraphEdge(v1, v2)

    graphLayoutView = vtkGraphLayoutView()
    graphLayoutView.AddRepresentationFromInput(g)
    graphLayoutView.SetLayoutStrategy('Simple 2D')
    graphLayoutView.ResetCamera()
    graphLayoutView.Render()

    graphLayoutView.GetLayoutStrategy().SetRandomSeed(0)

    graphLayoutView.GetInteractor().Start()


if __name__ == '__main__':
    main()
