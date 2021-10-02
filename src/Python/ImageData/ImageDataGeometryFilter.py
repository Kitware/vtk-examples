#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersGeometry import vtkImageDataGeometryFilter
from vtkmodules.vtkImagingSources import vtkImageCanvasSource2D
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create an image
    source1 = vtkImageCanvasSource2D()
    source1.SetScalarTypeToUnsignedChar()
    source1.SetNumberOfScalarComponents(3)
    source1.SetExtent(0, 100, 0, 100, 0, 0)
    source1.SetDrawColor(colors.GetColor4ub('SteelBlue'))
    source1.FillBox(0, 100, 0, 100)
    source1.SetDrawColor(colors.GetColor4ub('PaleGoldenrod'))
    source1.FillBox(10, 20, 10, 20)
    source1.FillBox(40, 50, 20, 30)
    source1.Update()

    # Convert the image to a polydata
    imageDataGeometryFilter = vtkImageDataGeometryFilter()
    imageDataGeometryFilter.SetInputConnection(source1.GetOutputPort())
    imageDataGeometryFilter.Update()

    # Create a mapper and actor
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(imageDataGeometryFilter.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)

    # Visualization
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('RosyBrown'))
    renderWindow.SetWindowName('ImageDataGeometryFilter')
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
