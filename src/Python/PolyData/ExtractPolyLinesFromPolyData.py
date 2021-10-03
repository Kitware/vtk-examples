#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkIdList
from vtkmodules.vtkCommonDataModel import vtkPlane
from vtkmodules.vtkFiltersCore import (
    vtkCutter,
    vtkStripper
)
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()
    lineColor = colors.GetColor3d('peacock')
    modelColor = colors.GetColor3d('silver')
    backgroundColor = colors.GetColor3d('wheat')

    modelSource = vtkSphereSource()

    plane = vtkPlane()

    cutter = vtkCutter()
    cutter.SetInputConnection(modelSource.GetOutputPort())
    cutter.SetCutFunction(plane)
    cutter.GenerateValues(10, -0.5, 0.5)

    modelMapper = vtkPolyDataMapper()
    modelMapper.SetInputConnection(modelSource.GetOutputPort())

    model = vtkActor()
    model.SetMapper(modelMapper)
    model.GetProperty().SetDiffuseColor(modelColor)
    model.GetProperty().SetInterpolationToFlat()

    stripper = vtkStripper()
    stripper.SetInputConnection(cutter.GetOutputPort())
    stripper.JoinContiguousSegmentsOn()

    linesMapper = vtkPolyDataMapper()
    linesMapper.SetInputConnection(stripper.GetOutputPort())

    lines = vtkActor()
    lines.SetMapper(linesMapper)
    lines.GetProperty().SetDiffuseColor(lineColor)
    lines.GetProperty().SetLineWidth(3.)

    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()

    renderWindow.AddRenderer(renderer)
    renderWindow.SetSize(640, 480)
    renderWindow.SetWindowName('ExtractPolyLinesFromPolyData')

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    # Add the actors to the renderer.
    renderer.AddActor(model)
    renderer.AddActor(lines)
    renderer.SetBackground(backgroundColor)
    renderer.GetActiveCamera().Azimuth(-45)
    renderer.GetActiveCamera().Elevation(-22.5)
    renderer.ResetCamera()

    # This starts the event loop and as a side effect causes an
    # initial render.
    renderWindow.Render()
    interactor.Start()

    # Extract the lines from the polydata.
    numberOfLines = cutter.GetOutput().GetNumberOfLines()

    print('-----------Lines without using vtkStripper')
    print('There are {0} lines in the polydata'.format(numberOfLines))

    numberOfLines = stripper.GetOutput().GetNumberOfLines()
    points = stripper.GetOutput().GetPoints()
    cells = stripper.GetOutput().GetLines()
    cells.InitTraversal()

    print('-----------Lines using vtkStripper')
    print('There are {0} lines in the polydata'.format(numberOfLines))

    indices = vtkIdList()
    lineCount = 0

    while cells.GetNextCell(indices):
        print('Line {0}:'.format(lineCount))
        for i in range(indices.GetNumberOfIds()):
            point = points.GetPoint(indices.GetId(i))
            print('\t({0:0.6f} ,{1:0.6f}, {2:0.6f})'.format(point[0], point[1], point[2]))
        lineCount += 1


if __name__ == '__main__':
    main()
