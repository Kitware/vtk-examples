#!/usr/bin/e

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2

from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkCommand,
    vtkVersion
)
from vtkmodules.vtkFiltersSources import vtkCylinderSource
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkInteractionWidgets import (
    vtkCameraOrientationWidget,
    vtkSplineWidget
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main(argv):
    colors = vtkNamedColors()
    colors.SetColor('ParaViewBkg', [82, 87, 110, 255])

    has_cow = False
    if vtk_version_ok(9, 0, 20210718):
        cam_orient_manipulator = vtkCameraOrientationWidget()
        has_cow = True

    window_width = 1024
    window_height = 1024

    ren_win = vtkRenderWindow()
    ren_win.SetSize(window_width, window_height)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)
    style = vtkInteractorStyleTrackballCamera()
    iren.SetInteractorStyle(style)

    renderer = vtkRenderer()
    renderer.SetBackground(colors.GetColor3d('ParaViewBkg'))

    # Create a cylinder.
    cylinder = vtkCylinderSource()
    cylinder.SetCenter(0.0, 0.0, 0.0)
    cylinder.SetRadius(3.0)
    cylinder.SetHeight(5.0)
    cylinder.SetResolution(100)

    # Create a mapper and actor
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(cylinder.GetOutputPort())
    actor = vtkActor()
    actor.GetProperty().SetColor(colors.GetColor3d('Cornsilk'))
    actor.SetMapper(mapper)
    # Add the actor to the scene
    renderer.AddActor(actor)

    ren_win.AddRenderer(renderer)

    # A spline widget
    spline_widget = vtkSplineWidget()
    spline_widget.SetInteractor(iren)
    spline_widget.SetProp3D(actor)
    spline_widget.PlaceWidget(-2.5, 2.5, 3.5, 3.5, 0, 0, )
    spline_widget.On()

    spline_widget.AddObserver(vtkCommand.EndInteractionEvent, SplineCallback(spline_widget))
    # Important: The interactor must be set prior to enabling the widget.
    iren.SetRenderWindow(ren_win)

    if has_cow:
        cam_orient_manipulator.SetParentRenderer(renderer)
        # Enable the widget.
        cam_orient_manipulator.On()

    ren_win.Render()
    ren_win.SetWindowName('SplineWidget')
    iren.Start()


def vtk_version_ok(major, minor, build):
    """
    Check the VTK version.

    :param major: Requested major version.
    :param minor: Requested minor version.
    :param build: Requested build version.
    :return: True if the requested VTK version is >= the actual VTK version.
    """
    requested_version = (100 * int(major) + int(minor)) * 100000000 + int(build)
    ver = vtkVersion()
    actual_version = (100 * ver.GetVTKMajorVersion() + ver.GetVTKMinorVersion()) \
                     * 100000000 + ver.GetVTKBuildVersion()
    if actual_version >= requested_version:
        return True
    else:
        return False


class SplineCallback:
    def __init__(self, spline_widget):
        self.spline = spline_widget

    def __call__(self, caller, ev):
        spline_widget = caller
        length = spline_widget.GetSummedLength()
        print(f'Length: {length}')


if __name__ == '__main__':
    import sys

    main(sys.argv)
