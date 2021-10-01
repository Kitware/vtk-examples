#!/usr/bin/python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
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

    # Create a set of heights on a grid.
    # This is often called a "terrain map".
    points = vtkPoints()

    gridSize = 10
    for x in range(gridSize):
        for y in range(gridSize):
            points.InsertNextPoint(x, y, int((x + y) / (y + 1)))

    # Add the grid points to a polydata object
    polydata = vtkPolyData()
    polydata.SetPoints(points)

    delaunay = vtkDelaunay2D()
    delaunay.SetInputData(polydata)

    # Visualize
    meshMapper = vtkPolyDataMapper()
    meshMapper.SetInputConnection(delaunay.GetOutputPort())

    meshActor = vtkActor()
    meshActor.SetMapper(meshMapper)
    meshActor.GetProperty().SetColor(colors.GetColor3d('Banana'))
    meshActor.GetProperty().EdgeVisibilityOn()

    glyphFilter = vtkVertexGlyphFilter()
    glyphFilter.SetInputData(polydata)

    pointMapper = vtkPolyDataMapper()
    pointMapper.SetInputConnection(glyphFilter.GetOutputPort())

    pointActor = vtkActor()
    pointActor.GetProperty().SetColor(colors.GetColor3d('Tomato'))
    pointActor.GetProperty().SetPointSize(5)
    pointActor.SetMapper(pointMapper)

    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(meshActor)
    renderer.AddActor(pointActor)
    renderer.SetBackground(colors.GetColor3d('Mint'))

    renderWindowInteractor.Initialize()
    renderWindow.SetWindowName('Delaunay2D')
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
