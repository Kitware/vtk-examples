#!/usr/bin/env python

import vtkmodules.all as vtk


def main():
    colors = vtk.vtkNamedColors()

    # Create a grid    
    grid = vtk.vtkRectilinearGrid()

    grid.SetDimensions(2, 3, 2)

    xArray = vtk.vtkDoubleArray()
    xArray.InsertNextValue(0.0)
    xArray.InsertNextValue(2.0)

    yArray = vtk.vtkDoubleArray()
    yArray.InsertNextValue(0.0)
    yArray.InsertNextValue(1.0)
    yArray.InsertNextValue(2.0)

    zArray = vtk.vtkDoubleArray()
    zArray.InsertNextValue(0.0)
    zArray.InsertNextValue(5.0)

    grid.SetXCoordinates(xArray)
    grid.SetYCoordinates(yArray)
    grid.SetZCoordinates(zArray)

    shrinkFilter = vtk.vtkShrinkFilter()
    shrinkFilter.SetInputData(grid)
    shrinkFilter.SetShrinkFactor(.8)

    # Create a mapper and actor
    mapper = vtk.vtkDataSetMapper()
    mapper.SetInputConnection(shrinkFilter.GetOutputPort())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('PeachPuff'))

    # Visualize
    renderer = vtk.vtkRenderer()
    renderWindow = vtk.vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('VisualizeRectilinearGrid')

    renderWindowInteractor = vtk.vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))
    renderer.GetActiveCamera().Roll(10.0)
    renderer.GetActiveCamera().Elevation(60.0)
    renderer.GetActiveCamera().Azimuth(30.0)
    renderer.ResetCamera()

    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
