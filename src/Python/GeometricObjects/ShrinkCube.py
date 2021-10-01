#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersGeneral import vtkShrinkFilter
from vtkmodules.vtkFiltersSources import vtkCubeSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkProperty,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create a cube.
    cubeSource = vtkCubeSource()

    shrink = vtkShrinkFilter()
    shrink.SetInputConnection(cubeSource.GetOutputPort())
    shrink.SetShrinkFactor(0.9)

    # Create a mapper and actor.

    mapper = vtkDataSetMapper()
    mapper.SetInputConnection(shrink.GetOutputPort())

    back = vtkProperty()
    back.SetColor(colors.GetColor3d('Tomato'))

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().EdgeVisibilityOn()
    actor.GetProperty().SetColor(colors.GetColor3d('Banana'))
    actor.SetBackfaceProperty(back)

    # Create a renderer, render window, and interactor.
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actors to the scene
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('Silver'))

    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(30)
    renderer.GetActiveCamera().Elevation(30)
    renderer.ResetCameraClippingRange()

    # Render and interact
    renderWindow.SetWindowName('ShrinkCube')
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
