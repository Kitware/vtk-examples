#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkImagingCore import vtkImageCast
from vtkmodules.vtkImagingMath import vtkImageWeightedSum
from vtkmodules.vtkImagingSources import (
    vtkImageMandelbrotSource,
    vtkImageSinusoidSource
)
from vtkmodules.vtkRenderingCore import (
    vtkImageActor,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create image 1
    source1 = vtkImageMandelbrotSource()
    source1.SetWholeExtent(0, 255, 0, 255, 0, 0)
    source1.Update()

    source1Double = vtkImageCast()
    source1Double.SetInputConnection(0, source1.GetOutputPort())
    source1Double.SetOutputScalarTypeToDouble()

    # Create image 2
    source2 = vtkImageSinusoidSource()
    source2.SetWholeExtent(0, 255, 0, 255, 0, 0)
    source2.Update()

    # Do the sum
    sumFilter = vtkImageWeightedSum()
    sumFilter.SetWeight(0, 0.8)
    sumFilter.SetWeight(1, 0.2)
    sumFilter.AddInputConnection(source1Double.GetOutputPort())
    sumFilter.AddInputConnection(source2.GetOutputPort())
    sumFilter.Update()

    # Display the images
    source1CastFilter = vtkImageCast()
    source1CastFilter.SetInputConnection(source1.GetOutputPort())
    source1CastFilter.SetOutputScalarTypeToUnsignedChar()
    source1CastFilter.Update()

    source2CastFilter = vtkImageCast()
    source2CastFilter.SetInputConnection(source2.GetOutputPort())
    source2CastFilter.SetOutputScalarTypeToUnsignedChar()
    source2CastFilter.Update()

    summedCastFilter = vtkImageCast()
    summedCastFilter.SetInputConnection(sumFilter.GetOutputPort())
    summedCastFilter.SetOutputScalarTypeToUnsignedChar()
    summedCastFilter.Update()

    # Create actors
    source1Actor = vtkImageActor()
    source1Actor.GetMapper().SetInputConnection(source1CastFilter.GetOutputPort())

    source2Actor = vtkImageActor()
    source2Actor.GetMapper().SetInputConnection(source2CastFilter.GetOutputPort())

    summedActor = vtkImageActor()
    summedActor.GetMapper().SetInputConnection(summedCastFilter.GetOutputPort())

    # There will be one render window
    renderWindow = vtkRenderWindow()
    renderWindow.SetSize(600, 300)

    # And one interactor
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    # Define viewport ranges
    # (xmin, ymin, xmax, ymax)
    leftViewport = [0.0, 0.0, 0.33, 1.0]
    centerViewport = [0.33, 0.0, .66, 1.0]
    rightViewport = [0.66, 0.0, 1.0, 1.0]

    # Setup renderers
    leftRenderer = vtkRenderer()
    renderWindow.AddRenderer(leftRenderer)
    leftRenderer.SetViewport(leftViewport)
    leftRenderer.SetBackground(colors.GetColor3d('Peru'))

    centerRenderer = vtkRenderer()
    renderWindow.AddRenderer(centerRenderer)
    centerRenderer.SetViewport(centerViewport)
    centerRenderer.SetBackground(colors.GetColor3d('DarkTurquoise'))

    rightRenderer = vtkRenderer()
    renderWindow.AddRenderer(rightRenderer)
    rightRenderer.SetViewport(rightViewport)
    rightRenderer.SetBackground(colors.GetColor3d('SteelBlue'))

    leftRenderer.AddActor(source1Actor)
    centerRenderer.AddActor(source2Actor)
    rightRenderer.AddActor(summedActor)

    leftRenderer.ResetCamera()
    centerRenderer.ResetCamera()
    rightRenderer.ResetCamera()

    renderWindow.SetWindowName('ImageWeightedSum')
    renderWindow.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
