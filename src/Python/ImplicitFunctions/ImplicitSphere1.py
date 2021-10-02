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

    sphere = vtkSphere()
    sphere.SetCenter(0, 0, 0)
    sphere.SetRadius(0.5)

    # The sample function generates a distance function from the implicit
    # function. This is then contoured to get a polygonal surface.
    sample = vtkSampleFunction()
    sample.SetImplicitFunction(sphere)
    sample.SetModelBounds(-.5, .5, -.5, .5, -.5, .5)
    sample.SetSampleDimensions(20, 20, 20)
    sample.ComputeNormalsOff()

    # contour
    surface = vtkContourFilter()
    surface.SetInputConnection(sample.GetOutputPort())
    surface.SetValue(0, 0.0)

    # mapper
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(surface.GetOutputPort())
    mapper.ScalarVisibilityOff()
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().EdgeVisibilityOn()
    actor.GetProperty().SetColor(colors.GetColor3d('AliceBlue'))
    actor.GetProperty().SetEdgeColor(colors.GetColor3d('SteelBlue'))

    # A renderer and render window
    renderer = vtkRenderer()
    renderer.SetBackground(colors.GetColor3d('Silver'))

    # add the actor
    renderer.AddActor(actor)

    # render window
    renwin = vtkRenderWindow()
    renwin.AddRenderer(renderer)
    renwin.SetWindowName('ImplicitSphere1')

    # An interactor
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renwin)

    # Start
    interactor.Initialize()
    renwin.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
