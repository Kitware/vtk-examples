#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkMinimalStandardRandomSequence,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import vtkPolyData
from vtkmodules.vtkFiltersCore import vtkDelaunay2D
from vtkmodules.vtkFiltersGeneral import vtkVertexGlyphFilter
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()
    # Create points on an XY grid with random Z coordinate
    points = vtkPoints()
    gridSize = 10
    seed = 0
    randomSequence = vtkMinimalStandardRandomSequence()
    randomSequence.Initialize(seed)
    for x in range(0, gridSize):
        for y in range(0, gridSize):
            d = randomSequence.GetValue()
            randomSequence.Next()
            points.InsertNextPoint(x, y, d * 3)

    # Add the grid points to a polydata object
    polydata = vtkPolyData()
    polydata.SetPoints(points)

    glyphFilter = vtkVertexGlyphFilter()
    glyphFilter.SetInputData(polydata)
    glyphFilter.Update()

    # Create a mapper and actor
    pointsMapper = vtkPolyDataMapper()
    pointsMapper.SetInputConnection(glyphFilter.GetOutputPort())

    pointsActor = vtkActor()
    pointsActor.SetMapper(pointsMapper)
    pointsActor.GetProperty().SetPointSize(3)
    pointsActor.GetProperty().SetColor(colors.GetColor3d("Red"))

    # Triangulate the grid points
    delaunay = vtkDelaunay2D()
    delaunay.SetInputData(polydata)
    delaunay.Update()

    # Create a mapper and actor
    triangulatedMapper = vtkPolyDataMapper()
    triangulatedMapper.SetInputConnection(delaunay.GetOutputPort())

    triangulatedActor = vtkActor()
    triangulatedActor.SetMapper(triangulatedMapper)

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actor to the scene
    renderer.AddActor(pointsActor)
    renderer.AddActor(triangulatedActor)
    renderer.SetBackground(colors.GetColor3d("Green"))  # Background color green

    # Render and interact
    renderWindow.SetWindowName('TriangulateTerrainMap')
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
