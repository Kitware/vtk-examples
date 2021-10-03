#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkElevationFilter
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
    renWin = vtkRenderWindow()
    renWin.AddRenderer(renderer)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    sphere = vtkSphereSource()
    sphere.SetPhiResolution(12)
    sphere.SetThetaResolution(12)

    colorIt = vtkElevationFilter()
    colorIt.SetInputConnection(sphere.GetOutputPort())
    colorIt.SetLowPoint(0, 0, -1)
    colorIt.SetHighPoint(0, 0, 1)

    mapper = vtkDataSetMapper()
    mapper.SetInputConnection(colorIt.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))
    renWin.SetSize(640, 480)
    renWin.SetWindowName('ColoredSphere')

    renWin.Render()

    # Interact with the data.
    iren.Start()


if __name__ == '__main__':
    main()
