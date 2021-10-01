#!/usr/bin/env python

import vtkmodules.all as vtk


def main():
    colors = vtk.vtkNamedColors()

    # Create a grid
    grid = vtk.vtkRectilinearGrid()
    grid.SetDimensions(2, 3, 1)

    xArray = vtk.vtkDoubleArray()
    xArray.InsertNextValue(0.0)
    xArray.InsertNextValue(2.0)

    yArray = vtk.vtkDoubleArray()
    yArray.InsertNextValue(0.0)
    yArray.InsertNextValue(1.0)
    yArray.InsertNextValue(2.0)

    zArray = vtk.vtkDoubleArray()
    zArray.InsertNextValue(0.0)

    grid.SetXCoordinates(xArray)
    grid.SetYCoordinates(yArray)
    grid.SetZCoordinates(zArray)

    print('There are', grid.GetNumberOfPoints(), 'points.')
    print('There are', grid.GetNumberOfCells(), 'cells.')

    for id in range(0, grid.GetNumberOfPoints()):
        p = [0] * 3
        p = grid.GetPoint(id)
        print('Point', id, ':(', p[0], ',', p[1], ',', p[2], ')')

    # Create a mapper and actor
    mapper = vtk.vtkDataSetMapper()
    mapper.SetInputData(grid)

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('PeachPuff'))

    # Visualize
    renderer = vtk.vtkRenderer()
    renderWindow = vtk.vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('RectilinearGrid')

    renderWindowInteractor = vtk.vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))

    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
