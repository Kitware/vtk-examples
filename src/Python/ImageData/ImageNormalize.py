#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkImagingCore import vtkImageCast
from vtkmodules.vtkImagingGeneral import vtkImageNormalize
from vtkmodules.vtkImagingSources import vtkImageSinusoidSource
from vtkmodules.vtkRenderingCore import (
    vtkImageActor,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create an image
    source = vtkImageSinusoidSource()
    source.Update()

    normalizeFilter = vtkImageNormalize()

    normalizeFilter.SetInputConnection(source.GetOutputPort())
    normalizeFilter.Update()

    inputCastFilter = vtkImageCast()
    inputCastFilter.SetInputConnection(source.GetOutputPort())
    inputCastFilter.SetOutputScalarTypeToUnsignedChar()
    inputCastFilter.Update()

    normalizeCastFilter = vtkImageCast()
    normalizeCastFilter.SetInputConnection(normalizeFilter.GetOutputPort())
    normalizeCastFilter.SetOutputScalarTypeToUnsignedChar()
    normalizeCastFilter.Update()

    # Create actors
    inputActor = vtkImageActor()
    inputActor.GetMapper().SetInputConnection(inputCastFilter.GetOutputPort())

    normalizedActor = vtkImageActor()
    normalizedActor.GetMapper().SetInputConnection(normalizeCastFilter.GetOutputPort())

    # There will be one render window
    renderWindow = vtkRenderWindow()
    renderWindow.SetSize(600, 300)

    # And one interactor
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    # Define viewport ranges
    # (xmin, ymin, xmax, ymax)
    leftViewport = [0.0, 0.0, 0.5, 1.0]
    rightViewport = [0.5, 0.0, 1.0, 1.0]

    # Setup both renderers
    leftRenderer = vtkRenderer()
    renderWindow.AddRenderer(leftRenderer)
    leftRenderer.SetViewport(leftViewport)
    leftRenderer.SetBackground(colors.GetColor3d('Sienna'))

    rightRenderer = vtkRenderer()
    renderWindow.AddRenderer(rightRenderer)
    rightRenderer.SetViewport(rightViewport)
    rightRenderer.SetBackground(colors.GetColor3d('SteelBlue'))

    leftRenderer.AddActor(inputActor)
    rightRenderer.AddActor(normalizedActor)

    leftRenderer.ResetCamera()

    rightRenderer.ResetCamera()

    renderWindow.SetWindowName('ImageNormalize')
    renderWindow.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
