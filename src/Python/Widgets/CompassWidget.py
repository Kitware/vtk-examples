#!/usr/bin/env python3

# determine if the current VTK version has the improved CompassWidget
import sys
sys.path.append('../Utilities')
from CheckVTKVersion import vtk_version_ok
VTK_IMPROVED_COMPASSWIDGET = vtk_version_ok(9, 2, 20220831)

import math
# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonCore import (
    vtkCommand,
    vtkMath
)
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkRenderingAnnotation import vtkAnnotatedCubeActor
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import (
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)

if VTK_IMPROVED_COMPASSWIDGET:
    from vtkmodules.vtkInteractionWidgets import (
        vtkCompassRepresentation,
        vtkCompassWidget
    )
else:
    from vtkmodules.vtkGeovisCore import (
        vtkCompassRepresentation,
        vtkCompassWidget
    )


def CompassWidgetValueChangedCallback(widget, event):
    """
    Callback to set the camera position according to the position parameters given by the vtkCompassWidget.
    """
    try:
        camera = widget.GetCurrentRenderer().GetActiveCamera()
    except AttributeError:
        return
    
    # calculate new camera position from compass widget parameters
    distance = widget.GetDistance()
    tilt = widget.GetTilt()
    heading = widget.GetHeading()
    
    x = distance * math.cos(vtkMath.RadiansFromDegrees(heading)) * math.cos(vtkMath.RadiansFromDegrees(tilt))
    y = distance * math.sin(vtkMath.RadiansFromDegrees(heading)) * math.cos(vtkMath.RadiansFromDegrees(tilt))
    z = distance * math.sin(vtkMath.RadiansFromDegrees(tilt))
    
    camera.SetPosition(x, y, z)
    camera.SetFocalPoint(0, 0, 0)
    camera.SetViewUp(0, 0, 1)
    camera.SetClippingRange(0.1, distance + 1)
    
    widget.GetCurrentRenderer().Render()



def main():
    colors = vtkNamedColors()

    actor = vtkAnnotatedCubeActor()
    actor.GetCubeProperty().SetColor(colors.GetColor3d('PeachPuff'))

    # a renderer and render window
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)

    # an interactor
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # create the widget and its representation
    compassRepresentation = vtkCompassRepresentation()

    compassWidget = vtkCompassWidget()
    compassWidget.SetInteractor(renderWindowInteractor)
    compassWidget.SetRepresentation(compassRepresentation)
    
    # add a callback to update the camera position on vtkCommand::WidgetValueChangedEvent
    compassWidget.AddObserver(vtkCommand.WidgetValueChangedEvent, CompassWidgetValueChangedCallback)

    if VTK_IMPROVED_COMPASSWIDGET:
        compassRepresentation.SetMinimumDistance(2)
        compassRepresentation.SetMaximumDistance(10)
        compassWidget.SetDistance(5)
        compassWidget.SetTiltSpeed(45)
        compassWidget.SetDistanceSpeed(2)
        
    # add the actors to the scene
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('MidnightBlue'))

    renderWindow.SetSize(640, 480)
    renderWindow.SetWindowName('CompassWidget')

    renderWindow.Render()
    compassWidget.EnabledOn()

    if VTK_IMPROVED_COMPASSWIDGET:
        # no interactor style - camera is moved by widget callback
        renderWindowInteractor.SetInteractorStyle(None)
        # set camera to initial position
        compassWidget.InvokeEvent(vtkCommand.WidgetValueChangedEvent)

    # begin interaction
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
