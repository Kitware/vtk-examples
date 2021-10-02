#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkSphere
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

    # Set the background color.
    colors.SetColor("BkgColor", [51, 77, 102, 255])

    sphere = vtkSphere()

    # Sample the function
    sample = vtkSampleFunction()
    sample.SetSampleDimensions(50, 50, 50)
    sample.SetImplicitFunction(sphere)
    value = 2.0
    xmin = -value
    xmax = value
    ymin = -value
    ymax = value
    zmin = -value
    zmax = value
    sample.SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax)

    # Create the 0 isosurface
    contours = vtkContourFilter()
    contours.SetInputConnection(sample.GetOutputPort())
    contours.GenerateValues(1, 1, 1)

    # Map the contours to graphical primitives
    contourMapper = vtkPolyDataMapper()
    contourMapper.SetInputConnection(contours.GetOutputPort())
    contourMapper.ScalarVisibilityOff()

    # Create an actor for the contours
    contourActor = vtkActor()
    contourActor.SetMapper(contourMapper)

    # Visualize
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('ImplicitSphere')

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    renderer.AddActor(contourActor)
    renderer.SetBackground(colors.GetColor3d("BkgColor"))

    renderWindow.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
