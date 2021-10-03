#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkDoubleArray
from vtkmodules.vtkCommonDataModel import vtkRectilinearGrid
from vtkmodules.vtkFiltersGeneral import vtkShrinkFilter
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create a grid    
    grid = vtkRectilinearGrid()

    grid.SetDimensions(2, 3, 2)

    xArray = vtkDoubleArray()
    xArray.InsertNextValue(0.0)
    xArray.InsertNextValue(2.0)

    yArray = vtkDoubleArray()
    yArray.InsertNextValue(0.0)
    yArray.InsertNextValue(1.0)
    yArray.InsertNextValue(2.0)

    zArray = vtkDoubleArray()
    zArray.InsertNextValue(0.0)
    zArray.InsertNextValue(5.0)

    grid.SetXCoordinates(xArray)
    grid.SetYCoordinates(yArray)
    grid.SetZCoordinates(zArray)

    shrinkFilter = vtkShrinkFilter()
    shrinkFilter.SetInputData(grid)
    shrinkFilter.SetShrinkFactor(.8)

    # Create a mapper and actor
    mapper = vtkDataSetMapper()
    mapper.SetInputConnection(shrinkFilter.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('PeachPuff'))

    # Visualize
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('VisualizeRectilinearGrid')

    renderWindowInteractor = vtkRenderWindowInteractor()
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
