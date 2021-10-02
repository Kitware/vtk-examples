#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkPerlinNoise
from vtkmodules.vtkFiltersCore import vtkContourFilter
from vtkmodules.vtkImagingHybrid import vtkSampleFunction
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()
    perlinNoise = vtkPerlinNoise()
    perlinNoise.SetFrequency(2, 1.25, 1.5)
    perlinNoise.SetPhase(0, 0, 0)

    sample = vtkSampleFunction()
    sample.SetImplicitFunction(perlinNoise)
    sample.SetSampleDimensions(65, 65, 20)
    sample.ComputeNormalsOff()

    surface = vtkContourFilter()
    surface.SetInputConnection(sample.GetOutputPort())
    surface.SetValue(0, 0.0)

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(surface.GetOutputPort())
    mapper.ScalarVisibilityOff()

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('SteelBlue'))

    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    # Add the actors to the renderer, set the background and size
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))

    renderWindow.SetWindowName('PerlinNoise')
    renderWindow.SetSize(300, 300)
    renderer.ResetCamera()
    renderWindow.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
