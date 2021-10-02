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
    vtkHexahedron,
    vtkUnstructuredGrid
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

    # Set the background color.
    colors.SetColor("BkgColor", [51, 77, 102, 255])

    # For the hexahedron setup the coordinates of eight points.
    # The two faces must be in counter clockwise order as viewed from the
    # outside.
    pointCoordinates = list()
    pointCoordinates.append([0.0, 0.0, 0.0])  # Face 1
    pointCoordinates.append([1.0, 0.0, 0.0])
    pointCoordinates.append([1.0, 1.0, 0.0])
    pointCoordinates.append([0.0, 1.0, 0.0])
    pointCoordinates.append([0.0, 0.0, 1.0])  # Face 2
    pointCoordinates.append([1.0, 0.0, 1.0])
    pointCoordinates.append([1.0, 1.0, 1.0])
    pointCoordinates.append([0.0, 1.0, 1.0])

    # Create the points.
    points = vtkPoints()

    # Create a hexahedron from the points.
    hexahedron = vtkHexahedron()

    for i in range(0, len(pointCoordinates)):
        points.InsertNextPoint(pointCoordinates[i])
        hexahedron.GetPointIds().SetId(i, i)

    # Add the hexahedron to a cell array.
    hexs = vtkCellArray()
    hexs.InsertNextCell(hexahedron)

    # Add the points and hexahedron to an unstructured grid.
    uGrid = vtkUnstructuredGrid()
    uGrid.SetPoints(points)
    uGrid.InsertNextCell(hexahedron.GetCellType(), hexahedron.GetPointIds())

    # Visualize.
    mapper = vtkDataSetMapper()
    mapper.SetInputData(uGrid)

    actor = vtkActor()
    actor.GetProperty().SetColor(colors.GetColor3d("PeachPuff"))
    actor.SetMapper(mapper)

    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName("Hexahedron")
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d("BkgColor"))
    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(30)
    renderer.GetActiveCamera().Elevation(30)

    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
