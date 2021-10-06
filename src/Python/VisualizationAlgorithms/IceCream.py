#!/usr/bin/env python

"""
This example demonstrates how to use boolean combinations of implicit
 functions to create a model of an ice cream cone.

"""
# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import (
    vtkCone,
    vtkImplicitBoolean,
    vtkPlane,
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

    # Create implicit function primitives. These have been carefully placed to
    # give the effect that we want. We are going to use various combinations of
    # these functions to create the shape we want for example, we use planes
    # intersected with a cone (which is infinite in extent) to get a finite
    # cone.
    #
    cone = vtkCone()
    cone.SetAngle(20)

    vertPlane = vtkPlane()
    vertPlane.SetOrigin(.1, 0, 0)
    vertPlane.SetNormal(-1, 0, 0)

    basePlane = vtkPlane()
    basePlane.SetOrigin(1.2, 0, 0)
    basePlane.SetNormal(1, 0, 0)

    iceCream = vtkSphere()
    iceCream.SetCenter(1.333, 0, 0)
    iceCream.SetRadius(0.5)

    bite = vtkSphere()
    bite.SetCenter(1.5, 0, 0.5)
    bite.SetRadius(0.25)

    # Combine primitives to build ice-cream cone. Clip the cone with planes.
    theCone = vtkImplicitBoolean()
    theCone.SetOperationTypeToIntersection()
    theCone.AddFunction(cone)
    theCone.AddFunction(vertPlane)
    theCone.AddFunction(basePlane)

    # Take a bite out of the ice cream.
    theCream = vtkImplicitBoolean()
    theCream.SetOperationTypeToDifference()
    theCream.AddFunction(iceCream)
    theCream.AddFunction(bite)

    # The sample function generates a distance function from the
    # implicit function (which in this case is the cone). This is
    # then contoured to get a polygonal surface.
    #
    theConeSample = vtkSampleFunction()
    theConeSample.SetImplicitFunction(theCone)
    theConeSample.SetModelBounds(-1, 1.5, -1.25, 1.25, -1.25, 1.25)
    theConeSample.SetSampleDimensions(128, 128, 128)
    theConeSample.ComputeNormalsOff()

    theConeSurface = vtkContourFilter()
    theConeSurface.SetInputConnection(theConeSample.GetOutputPort())
    theConeSurface.SetValue(0, 0.0)

    coneMapper = vtkPolyDataMapper()
    coneMapper.SetInputConnection(theConeSurface.GetOutputPort())
    coneMapper.ScalarVisibilityOff()

    coneActor = vtkActor()
    coneActor.SetMapper(coneMapper)
    coneActor.GetProperty().SetColor(colors.GetColor3d('Chocolate'))

    # The same here for the ice cream.
    #
    theCreamSample = vtkSampleFunction()
    theCreamSample.SetImplicitFunction(theCream)
    theCreamSample.SetModelBounds(0, 2.5, -1.25, 1.25, -1.25, 1.25)
    theCreamSample.SetSampleDimensions(128, 128, 128)
    theCreamSample.ComputeNormalsOff()

    theCreamSurface = vtkContourFilter()
    theCreamSurface.SetInputConnection(theCreamSample.GetOutputPort())
    theCreamSurface.SetValue(0, 0.0)

    creamMapper = vtkPolyDataMapper()
    creamMapper.SetInputConnection(theCreamSurface.GetOutputPort())
    creamMapper.ScalarVisibilityOff()

    creamActor = vtkActor()
    creamActor.SetMapper(creamMapper)
    creamActor.GetProperty().SetDiffuseColor(colors.GetColor3d('Mint'))
    creamActor.GetProperty().SetSpecular(.6)
    creamActor.GetProperty().SetSpecularPower(50)

    # Create the usual rendering stuff.
    #
    ren1 = vtkRenderer()

    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren1)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Add the actors to the renderer, set the background and size.
    #
    ren1.AddActor(coneActor)
    ren1.AddActor(creamActor)
    ren1.SetBackground(colors.GetColor3d('SlateGray'))
    renWin.SetSize(640, 480)
    renWin.SetWindowName('IceCream')

    ren1.ResetCamera()
    ren1.GetActiveCamera().Roll(90)
    ren1.GetActiveCamera().Dolly(1.25)
    ren1.ResetCameraClippingRange()
    iren.Initialize()

    # render the image
    #
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
