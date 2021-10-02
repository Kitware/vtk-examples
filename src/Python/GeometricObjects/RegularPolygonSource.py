#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersGeneral import vtkShrinkPolyData
from vtkmodules.vtkFiltersSources import vtkRegularPolygonSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkProperty,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create a pentagon
    polygonSource = vtkRegularPolygonSource()
    polygonSource.SetNumberOfSides(5)
    polygonSource.SetRadius(5)
    polygonSource.SetCenter(0, 0, 0)

    shrink = vtkShrinkPolyData()
    shrink.SetInputConnection(polygonSource.GetOutputPort())
    shrink.SetShrinkFactor(0.9)

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(shrink.GetOutputPort())

    back = vtkProperty()
    back.SetColor(colors.GetColor3d('Tomato'))

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().EdgeVisibilityOn()
    actor.GetProperty().SetLineWidth(5)
    actor.GetProperty().SetColor(colors.GetColor3d('Banana'))
    actor.SetBackfaceProperty(back)

    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName('RegularPolygonSource')
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('Silver'))

    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
