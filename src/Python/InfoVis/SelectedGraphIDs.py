#!/usr/bin/env python

# This is a variation on VTK/Examples/Infovis/Python/selection.py
# which shows how to use a vtkAnnotationLink to view the contents
# of a selection from a vtkGraphLayoutView

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersGeneral import vtkAnnotationLink
from vtkmodules.vtkInfovisCore import vtkRandomGraphSource
from vtkmodules.vtkInfovisLayout import vtkSimple2DLayoutStrategy
from vtkmodules.vtkViewsCore import vtkViewTheme
from vtkmodules.vtkViewsInfovis import vtkGraphLayoutView


def main():
    colors = vtkNamedColors()

    source = vtkRandomGraphSource()
    source.DirectedOff()
    source.SetNumberOfVertices(100)
    source.SetEdgeProbability(0)  # Basically generates a tree
    source.SetUseEdgeProbability(True)
    source.SetStartWithTree(True)
    source.IncludeEdgeWeightsOn()

    # Create force directed layout
    strategy = vtkSimple2DLayoutStrategy()
    strategy.SetInitialTemperature(5)

    # Create a graph layout view
    view = vtkGraphLayoutView()
    view.AddRepresentationFromInputConnection(source.GetOutputPort())
    view.SetVertexLabelArrayName('vertex id')
    view.SetVertexLabelVisibility(True)
    view.SetVertexColorArrayName('vertex id')
    view.SetColorVertices(True)
    view.SetEdgeColorArrayName('edge weight')
    view.SetColorEdges(True)
    view.SetLayoutStrategy(strategy)

    # Make sure the views are using a pedigree id selection
    view.GetRepresentation(0).SetSelectionType(2)

    # Create a selection link and set both view to use it
    annotationLink = vtkAnnotationLink()
    view.GetRepresentation(0).SetAnnotationLink(annotationLink)

    def select_callback(caller, event):
        '''
        In this particular data representation the current selection in the
         annotation link should always contain two nodes: one for the edges and
         one for the vertices. Which is which is not consistent, so you need to
         check the FieldType of each SelectionNode

        :param caller:
        :param event:
        :return:
        '''

        sel = caller.GetCurrentSelection()

        for nn in range(sel.GetNumberOfNodes()):
            sel_ids = sel.GetNode(nn).GetSelectionList()
            field_type = sel.GetNode(nn).GetFieldType()
            if field_type == 3:
                print('Vertex selection Pedigree IDs')
            if field_type == 4:
                print('Edge selection Pedigree IDs')
            if sel_ids.GetNumberOfTuples() > 0:
                for ii in range(sel_ids.GetNumberOfTuples()):
                    print(int(sel_ids.GetTuple1(ii)))
            else:
                print('-- empty')

        print('')

    # AnnotationChangedEvent will fire when the selection is changed
    annotationLink.AddObserver('AnnotationChangedEvent', select_callback)

    # Set the theme on the view
    theme = vtkViewTheme.CreateMellowTheme()
    theme.SetLineWidth(5)
    theme.SetPointSize(10)
    theme.SetCellOpacity(0.99)
    theme.SetOutlineColor(colors.GetColor3d('Gray'))
    # Vertices
    theme.SetSelectedPointColor(colors.GetColor3d('DodgerBlue'))
    theme.SetPointHueRange(1.0, 1.0)
    theme.SetPointSaturationRange(1.0, 1.0)
    theme.SetPointValueRange(0.0, 1.0)
    # theme.SetPointAlphaRange(0.2, 0.8)
    # Edges
    theme.SetSelectedCellColor(colors.GetColor3d('LavenderBlush'))
    theme.SetCellHueRange(0.1, 0.1)
    theme.SetCellSaturationRange(0.2, 1.0)
    theme.SetCellValueRange(0.5, 1.0)
    # theme.SetPointAlphaRange(0.2, 0.8)
    view.ApplyViewTheme(theme)
    theme.FastDelete()

    view.GetRenderWindow().SetSize(600, 600)
    view.GetRenderWindow().SetWindowName('SelectedGraphIDs')
    view.ResetCamera()
    view.Render()
    view.GetInteractor().Start()


if __name__ == '__main__':
    main()
