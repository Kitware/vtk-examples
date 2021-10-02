#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import (
    vtkBox,
    vtkImplicitBoolean,
    vtkSphere
)
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

    # create a sphere
    sphere = vtkSphere()
    sphere.SetRadius(1)
    sphere.SetCenter(1, 0, 0)

    # create a box
    box = vtkBox()
    box.SetBounds(-1, 1, -1, 1, -1, 1)

    # combine the two implicit functions
    boolean = vtkImplicitBoolean()
    boolean.SetOperationTypeToDifference()
    # boolean.SetOperationTypeToUnion()
    # boolean.SetOperationTypeToIntersection()
    boolean.AddFunction(box)
    boolean.AddFunction(sphere)

    # The sample function generates a distance function from the implicit
    # function. This is then contoured to get a polygonal surface.
    sample = vtkSampleFunction()
    sample.SetImplicitFunction(boolean)
    sample.SetModelBounds(-1, 2, -1, 1, -1, 1)
    sample.SetSampleDimensions(40, 40, 40)
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
    renwin.SetWindowName('BooleanOperationImplicitFunctions')

    # An interactor
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renwin)

    # Start
    interactor.Initialize()
    renwin.Render()
    # renderer.GetActiveCamera().AddObserver('ModifiedEvent', CameraModifiedCallback)
    renderer.GetActiveCamera().SetPosition(5.0, -4.0, 1.6)
    renderer.GetActiveCamera().SetViewUp(0.1, 0.5, 0.9)
    renderer.GetActiveCamera().SetDistance(6.7)
    renwin.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
