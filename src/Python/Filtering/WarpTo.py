#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkTubeFilter
from vtkmodules.vtkFiltersGeneral import vtkWarpTo
from vtkmodules.vtkFiltersSources import vtkLineSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()
    # Create the RenderWindow, Renderer and both Actors
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Create a line
    lineSource = vtkLineSource()
    lineSource.SetPoint1(0.0, 0.0, 0.0)
    lineSource.SetPoint2(0.0, 1.0, 0.0)
    lineSource.SetResolution(20)
    lineSource.Update()

    # Create a tube (cylinder) around the line
    tubeFilter = vtkTubeFilter()
    tubeFilter.SetInputConnection(lineSource.GetOutputPort())
    tubeFilter.SetRadius(.01)  # default is .5
    tubeFilter.SetNumberOfSides(50)
    tubeFilter.Update()

    warpTo = vtkWarpTo()
    warpTo.SetInputConnection(tubeFilter.GetOutputPort())
    warpTo.SetPosition(10, 1, 0)
    warpTo.SetScaleFactor(5)
    warpTo.AbsoluteOn()

    mapper = vtkDataSetMapper()
    mapper.SetInputConnection(warpTo.GetOutputPort())
    mapper.ScalarVisibilityOff()

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('Gold'))

    renderer.SetBackground(colors.GetColor3d('Green'))
    renderer.AddActor(actor)

    renderWindow.SetWindowName('WarpTo')
    renderWindow.Render()

    renderWindowInteractor.Initialize()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
