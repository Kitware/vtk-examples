#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkElevationFilter
from vtkmodules.vtkFiltersGeneral import vtkShrinkFilter
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    renderer = vtkRenderer()
    renderer.GetCullers().RemoveAllItems()

    renWin = vtkRenderWindow()
    renWin.AddRenderer(renderer)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    sphere = vtkSphereSource()
    sphere.SetThetaResolution(12)
    sphere.SetPhiResolution(12)

    shrink = vtkShrinkFilter()
    shrink.SetInputConnection(sphere.GetOutputPort())
    shrink.SetShrinkFactor(0.9)

    colorIt = vtkElevationFilter()
    colorIt.SetInputConnection(shrink.GetOutputPort())
    colorIt.SetLowPoint(0, 0, -.5)
    colorIt.SetHighPoint(0, 0, .5)

    mapper = vtkDataSetMapper()
    mapper.SetInputConnection(colorIt.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('LavenderBlush'))
    renWin.SetSize(600, 600)
    renWin.SetWindowName('LoopShrink')

    renWin.Render()

    renderer.GetActiveCamera().Zoom(1.5)

    #  Interact with the data.
    iren.Start()


if __name__ == '__main__':
    main()
