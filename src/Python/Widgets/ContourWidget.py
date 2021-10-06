#!/usr/bin/python

import math
import sys

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkCommand,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData
)
from vtkmodules.vtkInteractionWidgets import (
    vtkContourWidget,
    vtkOrientedGlyphContourRepresentation,
    vtkWidgetEvent
)
from vtkmodules.vtkRenderingCore import (
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # colors.SetColor('bkg', [0.1, 0.2, 0.4, 1.0])

    # Create the RenderWindow, Renderer and both Actors

    renderer = vtkRenderer()
    renderer.SetBackground(colors.GetColor3d('MidnightBlue'))

    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('ContourWidget')
    renderWindow.SetSize(600, 600)

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    contourRep = vtkOrientedGlyphContourRepresentation()
    contourRep.GetLinesProperty().SetColor(colors.GetColor3d('Red'))

    contourWidget = vtkContourWidget()
    contourWidget.SetInteractor(interactor)
    contourWidget.SetRepresentation(contourRep)
    contourWidget.On()

    for arg in sys.argv:
        if '-Shift' == arg:
            contourWidget.GetEventTranslator().RemoveTranslation(
                vtkCommand.LeftButtonPressEvent)
            contourWidget.GetEventTranslator().SetTranslation(
                vtkCommand.LeftButtonPressEvent,
                vtkWidgetEvent.Translate)
        elif '-Scale' == arg:
            contourWidget.GetEventTranslator().RemoveTranslation(
                vtkCommand.LeftButtonPressEvent)
            contourWidget.GetEventTranslator().SetTranslation(
                vtkCommand.LeftButtonPressEvent,
                vtkWidgetEvent.Scale)

    pd = vtkPolyData()

    points = vtkPoints()

    num_pts = 21
    for i in range(0, num_pts):
        angle = 2.0 * math.pi * i / 20.0
        points.InsertPoint(i, 0.1 * math.cos(angle),
                           0.1 * math.sin(angle), 0.0)
        # lines.InsertNextCell(i)
    vertex_indices = list(range(0, num_pts))
    vertex_indices.append(0)
    lines = vtkCellArray()
    lines.InsertNextCell(num_pts + 1, vertex_indices)

    pd.SetPoints(points)
    pd.SetLines(lines)

    # contourWidget.Initialize(pd, 1)
    contourWidget.Initialize(pd, 1)
    contourWidget.Render()
    renderer.ResetCamera()
    renderWindow.Render()

    interactor.Initialize()
    interactor.Start()


if __name__ == '__main__':
    main()
