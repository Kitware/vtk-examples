#!/usr/bin/python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkMinimalStandardRandomSequence,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData,
    vtkPolygon
)
from vtkmodules.vtkFiltersCore import vtkDelaunay2D
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Generate a 10 x 10 grid of points
    points = vtkPoints()
    gridSize = 10
    seed = 0
    randomSequence = vtkMinimalStandardRandomSequence()
    randomSequence.Initialize(seed)
    for x in range(gridSize):
        for y in range(gridSize):
            d1 = randomSequence.GetValue() / 2.0 - 0.25
            randomSequence.Next()
            d2 = randomSequence.GetValue() / 2.0 - 0.25
            randomSequence.Next()
            points.InsertNextPoint(x + d1, y + d2, 0)

    aPolyData = vtkPolyData()
    aPolyData.SetPoints(points)

    # Create a cell array to store the polygon in
    aCellArray = vtkCellArray()

    # Define a polygonal hole with a clockwise polygon
    aPolygon = vtkPolygon()

    aPolygon.GetPointIds().InsertNextId(22)
    aPolygon.GetPointIds().InsertNextId(23)
    aPolygon.GetPointIds().InsertNextId(24)
    aPolygon.GetPointIds().InsertNextId(25)
    aPolygon.GetPointIds().InsertNextId(35)
    aPolygon.GetPointIds().InsertNextId(45)
    aPolygon.GetPointIds().InsertNextId(44)
    aPolygon.GetPointIds().InsertNextId(43)
    aPolygon.GetPointIds().InsertNextId(42)
    aPolygon.GetPointIds().InsertNextId(32)

    aCellArray.InsertNextCell(aPolygon)

    # Create a polydata to store the boundary. The points must be the
    # same as the points we will triangulate.
    boundary = vtkPolyData()
    boundary.SetPoints(aPolyData.GetPoints())
    boundary.SetPolys(aCellArray)

    # Triangulate the grid points
    delaunay = vtkDelaunay2D()
    delaunay.SetInputData(aPolyData)
    delaunay.SetSourceData(boundary)

    # Visualize
    meshMapper = vtkPolyDataMapper()
    meshMapper.SetInputConnection(delaunay.GetOutputPort())

    meshActor = vtkActor()
    meshActor.SetMapper(meshMapper)
    meshActor.GetProperty().EdgeVisibilityOn()
    meshActor.GetProperty().SetEdgeColor(colors.GetColor3d('Peacock'))
    meshActor.GetProperty().SetInterpolationToFlat()

    boundaryMapper = vtkPolyDataMapper()
    boundaryMapper.SetInputData(boundary)

    boundaryActor = vtkActor()
    boundaryActor.SetMapper(boundaryMapper)
    boundaryActor.GetProperty().SetColor(colors.GetColor3d('Raspberry'))
    boundaryActor.GetProperty().SetLineWidth(3)
    boundaryActor.GetProperty().EdgeVisibilityOn()
    boundaryActor.GetProperty().SetEdgeColor(colors.GetColor3d('Red'))
    boundaryActor.GetProperty().SetRepresentationToWireframe()

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actor to the scene
    renderer.AddActor(meshActor)
    renderer.AddActor(boundaryActor)
    renderer.SetBackground(colors.GetColor3d('Mint'))

    # Render and interact
    renderWindow.SetSize(640, 480)
    renderWindow.SetWindowName('ConstrainedDelaunay2D')
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
