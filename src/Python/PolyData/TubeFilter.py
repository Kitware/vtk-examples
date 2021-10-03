#!/usr/bin/env python

# This example creates a tube around a line.
# This is helpful because when you zoom the camera, 
# the thickness of a line remains constant, 
# while the thickness of a tube varies.


# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkTubeFilter
from vtkmodules.vtkFiltersSources import vtkLineSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create a line
    lineSource = vtkLineSource()
    lineSource.SetPoint1(1.0, 0.0, 0.0)
    lineSource.SetPoint2(.0, 1.0, 0.0)

    # Setup actor and mapper
    lineMapper = vtkPolyDataMapper()
    lineMapper.SetInputConnection(lineSource.GetOutputPort())

    lineActor = vtkActor()
    lineActor.SetMapper(lineMapper)
    lineActor.GetProperty().SetColor(colors.GetColor3d('Red'))

    # Create tube filter
    tubeFilter = vtkTubeFilter()
    tubeFilter.SetInputConnection(lineSource.GetOutputPort())
    tubeFilter.SetRadius(0.025)
    tubeFilter.SetNumberOfSides(50)
    tubeFilter.Update()

    # Setup actor and mapper
    tubeMapper = vtkPolyDataMapper()
    tubeMapper.SetInputConnection(tubeFilter.GetOutputPort())

    tubeActor = vtkActor()
    tubeActor.SetMapper(tubeMapper)
    # Make the tube have some transparency.
    tubeActor.GetProperty().SetOpacity(0.5)

    # Setup render window, renderer, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName('TubeFilter')
    renderWindow.AddRenderer(renderer)

    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)
    # Visualise the arrow
    renderer.AddActor(lineActor)
    renderer.AddActor(tubeActor)
    renderer.SetBackground(colors.GetColor3d('DarkSlateGray'))
    renderer.ResetCamera()

    renderWindow.SetSize(300, 300)
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
