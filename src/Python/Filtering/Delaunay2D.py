#!/usr/bin/python

import vtk


def main():
    colors = vtk.vtkNamedColors()

    # Create a set of heights on a grid.
    # This is often called a "terrain map".
    points = vtk.vtkPoints()

    gridSize = 10
    for x in range(gridSize):
        for y in range(gridSize):
            points.InsertNextPoint(x, y, int((x + y) / (y + 1)))

    # Add the grid points to a polydata object
    polydata = vtk.vtkPolyData()
    polydata.SetPoints(points)

    delaunay = vtk.vtkDelaunay2D()
    delaunay.SetInputData(polydata)

    # Visualize
    meshMapper = vtk.vtkPolyDataMapper()
    meshMapper.SetInputConnection(delaunay.GetOutputPort())

    meshActor = vtk.vtkActor()
    meshActor.SetMapper(meshMapper)
    meshActor.GetProperty().SetColor(colors.GetColor3d('Banana'))
    meshActor.GetProperty().EdgeVisibilityOn()

    glyphFilter = vtk.vtkVertexGlyphFilter()
    glyphFilter.SetInputData(polydata)

    pointMapper = vtk.vtkPolyDataMapper()
    pointMapper.SetInputConnection(glyphFilter.GetOutputPort())

    pointActor = vtk.vtkActor()
    pointActor.GetProperty().SetColor(colors.GetColor3d('Tomato'))
    pointActor.GetProperty().SetPointSize(5)
    pointActor.SetMapper(pointMapper)

    renderer = vtk.vtkRenderer()
    renderWindow = vtk.vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtk.vtkRenderWindowInteractor()
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
