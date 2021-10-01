#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkPolyData
from vtkmodules.vtkFiltersCore import vtkContourFilter
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkImagingHybrid import vtkGaussianSplatter
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    # Create points on a sphere
    sphereSource = vtkSphereSource()
    sphereSource.Update()

    colors = vtkNamedColors()

    polydata = vtkPolyData()
    polydata.SetPoints(sphereSource.GetOutput().GetPoints())

    splatter = vtkGaussianSplatter()
    splatter.SetInputData(polydata)
    splatter.SetSampleDimensions(50, 50, 50)
    splatter.SetRadius(0.5)
    splatter.ScalarWarpingOff()

    surface = vtkContourFilter()
    surface.SetInputConnection(splatter.GetOutputPort())
    surface.SetValue(0, 0.01)

    # Create a mapper and actor
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(surface.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)

    # Visualize
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('SteelBlue'))

    renderWindow.SetWindowName('GaussianSplat')
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
