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
    vtkPlane,
    vtkUnstructuredGrid
)
from vtkmodules.vtkFiltersCore import vtkCutter
from vtkmodules.vtkFiltersGeometry import vtkDataSetSurfaceFilter
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Setup the coordinates of eight points
    # (the two faces must be in counter clockwise order as viewed from the
    # outside)
    pointCoords = [
        [0.0, 0.0, 0.0],
        [1.0, 0.0, 0.0],
        [1.0, 1.0, 0.0],
        [0.0, 1.0, 0.0],
        [0.0, 0.0, 1.0],
        [1.0, 0.0, 1.0],
        [1.0, 1.0, 1.0],
        [0.0, 1.0, 1.0]
    ]

    # Create the points and a hexahedron from the points.
    points = vtkPoints()
    hexa = vtkHexahedron()
    for i, pointCoord in enumerate(pointCoords):
        points.InsertNextPoint(pointCoord)
        hexa.GetPointIds().SetId(i, i)

    # Add the hexahedron to a cell array.
    hexs = vtkCellArray()
    hexs.InsertNextCell(hexa)

    # Add the points and hexahedron to an unstructured grid.
    uGrid = vtkUnstructuredGrid()
    uGrid.SetPoints(points)
    uGrid.InsertNextCell(hexa.GetCellType(), hexa.GetPointIds())

    # Extract the outer (polygonal) surface.
    surface = vtkDataSetSurfaceFilter()
    surface.SetInputData(uGrid)
    surface.Update()

    aBeamMapper = vtkDataSetMapper()
    aBeamMapper.SetInputConnection(surface.GetOutputPort())
    aBeamActor = vtkActor()
    aBeamActor.SetMapper(aBeamMapper)
    aBeamActor.AddPosition(0, 0, 0)
    aBeamActor.GetProperty().SetColor(
        colors.GetColor3d('Yellow'))
    aBeamActor.GetProperty().SetOpacity(0.60)
    aBeamActor.GetProperty().EdgeVisibilityOn()
    aBeamActor.GetProperty().SetEdgeColor(
        colors.GetColor3d('Black'))
    aBeamActor.GetProperty().SetLineWidth(1.5)

    # Create a plane to cut, here it cuts in the XZ direction
    # (xz normal=(1,0,0) XY =(0,0,1), YZ =(0,1,0)
    plane = vtkPlane()
    plane.SetOrigin(0.5, 0, 0)
    plane.SetNormal(1, 0, 0)

    # Create cutter
    cutter = vtkCutter()
    cutter.SetCutFunction(plane)
    cutter.SetInputData(aBeamActor.GetMapper().GetInput())
    cutter.Update()
    cutterMapper = vtkPolyDataMapper()
    cutterMapper.SetInputConnection(cutter.GetOutputPort())

    # Create plane actor
    planeActor = vtkActor()
    planeActor.GetProperty().SetColor(
        colors.GetColor3d('Red'))
    planeActor.GetProperty().SetLineWidth(2)
    planeActor.SetMapper(cutterMapper)

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName('DatasetSurface')
    renderWindow.AddRenderer(renderer)

    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actors to the scene
    renderer.AddActor(aBeamActor)
    renderer.AddActor(planeActor)
    renderer.SetBackground(
        colors.GetColor3d('Seashell'))

    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(-25)
    renderer.GetActiveCamera().Elevation(30)

    # Render and interact
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
