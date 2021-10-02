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
    vtkVertex
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)

    vertex = vtkVertex()
    vertex.GetPointIds().SetId(0, 0)

    vertices = vtkCellArray()
    vertices.InsertNextCell(vertex)

    polydata = vtkPolyData()
    polydata.SetPoints(points)
    polydata.SetVerts(vertices)

    # Setup actor and mapper
    mapper = vtkPolyDataMapper()
    mapper.SetInputData(polydata)
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetPointSize(30)
    actor.GetProperty().SetColor(colors.GetColor3d('PeachPuff'))

    # Setup render window, renderer, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName('Vertex')
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('DarkGreen'))

    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
