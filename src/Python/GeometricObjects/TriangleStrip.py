#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData,
    vtkTriangleStrip
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(0, 1, 0)
    points.InsertNextPoint(1, 0, 0)
    points.InsertNextPoint(1.5, 1, 0)

    triangleStrip = vtkTriangleStrip()
    triangleStrip.GetPointIds().SetNumberOfIds(4)
    triangleStrip.GetPointIds().SetId(0, 0)
    triangleStrip.GetPointIds().SetId(1, 1)
    triangleStrip.GetPointIds().SetId(2, 2)
    triangleStrip.GetPointIds().SetId(3, 3)

    cells = vtkCellArray()
    cells.InsertNextCell(triangleStrip)

    polydata = vtkPolyData()
    polydata.SetPoints(points)
    polydata.SetStrips(cells)

    # Create an actor and mapper
    mapper = vtkDataSetMapper()
    mapper.SetInputData(polydata)
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('PeachPuff'))
    actor.GetProperty().SetRepresentationToWireframe()

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName('TriangleStrip')
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('DarkGreen'))
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
