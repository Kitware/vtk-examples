#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import (
    vtkCylinder,
    vtkImplicitBoolean,
    vtkSphere
)
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersCore import vtkClipPolyData
from vtkmodules.vtkFiltersSources import vtkPlaneSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Demonstrate the use of clipping on polygonal data
    #

    # create pipeline
    #
    plane = vtkPlaneSource()
    plane.SetXResolution(25)
    plane.SetYResolution(25)
    plane.SetOrigin(-1, -1, 0)
    plane.SetPoint1(1, -1, 0)
    plane.SetPoint2(-1, 1, 0)

    transformSphere = vtkTransform()
    transformSphere.Identity()
    transformSphere.Translate(0.4, -0.4, 0)
    transformSphere.Inverse()

    sphere = vtkSphere()
    sphere.SetTransform(transformSphere)
    sphere.SetRadius(.5)

    transformCylinder = vtkTransform()
    transformCylinder.Identity()
    transformCylinder.Translate(-0.4, 0.4, 0)
    transformCylinder.RotateZ(30)
    transformCylinder.RotateY(60)
    transformCylinder.RotateX(90)
    transformCylinder.Inverse()

    cylinder = vtkCylinder()
    cylinder.SetTransform(transformCylinder)
    cylinder.SetRadius(.3)

    boolean = vtkImplicitBoolean()
    boolean.AddFunction(cylinder)
    boolean.AddFunction(sphere)

    clipper = vtkClipPolyData()
    clipper.SetInputConnection(plane.GetOutputPort())
    clipper.SetClipFunction(boolean)
    clipper.GenerateClippedOutputOn()
    clipper.GenerateClipScalarsOn()
    clipper.SetValue(0)

    clipMapper = vtkPolyDataMapper()
    clipMapper.SetInputConnection(clipper.GetOutputPort())
    clipMapper.ScalarVisibilityOff()

    clipActor = vtkActor()
    clipActor.SetMapper(clipMapper)
    clipActor.GetProperty().SetDiffuseColor(colors.GetColor3d('MidnightBlue'))
    clipActor.GetProperty().SetRepresentationToWireframe()

    clipInsideMapper = vtkPolyDataMapper()
    clipInsideMapper.SetInputData(clipper.GetClippedOutput())
    clipInsideMapper.ScalarVisibilityOff()

    clipInsideActor = vtkActor()
    clipInsideActor.SetMapper(clipInsideMapper)
    clipInsideActor.GetProperty().SetDiffuseColor(colors.GetColor3d('LightBlue'))

    # Create graphics stuff
    #
    ren1 = vtkRenderer()

    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren1)
    renWin.SetWindowName('ClipSphereCylinder')

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Add the actors to the renderer, set the background and size
    #
    ren1.AddActor(clipActor)

    ren1.AddActor(clipInsideActor)
    ren1.SetBackground(colors.GetColor3d('Wheat'))
    ren1.ResetCamera()
    ren1.GetActiveCamera().Dolly(1.4)
    ren1.ResetCameraClippingRange()

    renWin.SetSize(640, 480)

    # render the image
    #
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
