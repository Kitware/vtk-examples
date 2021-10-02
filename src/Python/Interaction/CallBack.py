#!/usr/bin/env python

'''
Demonstrate the use of a callback.

We also add call data.
'''

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersModeling import vtkOutlineFilter
from vtkmodules.vtkFiltersSources import vtkConeSource
from vtkmodules.vtkInteractionWidgets import vtkOrientationMarkerWidget
from vtkmodules.vtkRenderingAnnotation import vtkAxesActor
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    #  Decide what approach to use.
    use_function_callback = True

    colors = vtkNamedColors()

    # Create the Renderer, RenderWindow and RenderWindowInteractor.
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Use a cone as a source.
    source = vtkConeSource()
    source.SetCenter(0, 0, 0)
    source.SetRadius(1)
    # Use the golden ratio for the height. Because we can!
    source.SetHeight(1.6180339887498948482)
    source.SetResolution(128)
    source.Update()

    # Pipeline
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(source.GetOutputPort())
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('peacock'))
    # Lighting
    actor.GetProperty().SetAmbient(0.3)
    actor.GetProperty().SetDiffuse(0.0)
    actor.GetProperty().SetSpecular(1.0)
    actor.GetProperty().SetSpecularPower(20.0)

    # Get an outline of the data set for context.
    outline = vtkOutlineFilter()
    outline.SetInputData(source.GetOutput())
    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())
    outlineActor = vtkActor()
    outlineActor.GetProperty().SetColor(colors.GetColor3d('Black'))
    outlineActor.SetMapper(outlineMapper)

    # Add the actors to the renderer, set the background and size.
    ren.AddActor(actor)
    ren.AddActor(outlineActor)
    ren.SetBackground(colors.GetColor3d('AliceBlue'))
    renWin.SetSize(512, 512)

    # Set up a nice camera position.
    camera = vtkCamera()
    camera.SetPosition(4.6, -2.0, 3.8)
    camera.SetFocalPoint(0.0, 0.0, 0.0)
    camera.SetClippingRange(3.2, 10.2)
    camera.SetViewUp(0.3, 1.0, 0.13)
    ren.SetActiveCamera(camera)

    renWin.Render()
    renWin.SetWindowName('CallBack')

    axes1 = MakeAxesActor()
    om1 = vtkOrientationMarkerWidget()
    om1.SetOrientationMarker(axes1)
    # Position lower left in the viewport.
    om1.SetViewport(0, 0, 0.2, 0.2)
    om1.SetInteractor(iren)
    om1.EnabledOn()
    om1.InteractiveOn()

    # Set up the callback.
    if use_function_callback:
        # We are going to output the camera position when the event is triggered
        #  so we add the active camera as an attribute.
        GetOrientation.cam = ren.GetActiveCamera()
        # Register the callback with the object that is observing.
        iren.AddObserver('EndInteractionEvent', GetOrientation)
    else:
        iren.AddObserver('EndInteractionEvent', OrientationObserver(ren.GetActiveCamera()))
        # Or:
        # observer = OrientationObserver(ren.GetActiveCamera())
        # iren.AddObserver('EndInteractionEvent', observer)

    iren.Initialize()
    iren.Start()


def GetOrientation(caller, ev):
    '''
    Print out the orientation.

    We must do this before we register the callback in the calling function.
        GetOrientation.cam = ren.GetActiveCamera()

    :param caller:
    :param ev: The event.
    :return:
    '''
    # Just do this to demonstrate who called callback and the event that triggered it.
    print(caller.GetClassName(), 'Event Id:', ev)
    # Now print the camera orientation.
    CameraOrientation(GetOrientation.cam)


class OrientationObserver(object):
    def __init__(self, cam):
        self.cam = cam

    def __call__(self, caller, ev):
        # Just do this to demonstrate who called callback and the event that triggered it.
        print(caller.GetClassName(), 'Event Id:', ev)
        # Now print the camera orientation.
        CameraOrientation(self.cam)


def CameraOrientation(cam):
    fmt1 = '{:>15s}'
    fmt2 = '{:9.6g}'
    print(fmt1.format('Position:'), ', '.join(map(fmt2.format, cam.GetPosition())))
    print(fmt1.format('Focal point:'), ', '.join(map(fmt2.format, cam.GetFocalPoint())))
    print(fmt1.format('Clipping range:'), ', '.join(map(fmt2.format, cam.GetClippingRange())))
    print(fmt1.format('View up:'), ', '.join(map(fmt2.format, cam.GetViewUp())))
    print(fmt1.format('Distance:'), fmt2.format(cam.GetDistance()))


def MakeAxesActor():
    axes = vtkAxesActor()
    axes.SetShaftTypeToCylinder()
    axes.SetXAxisLabelText('X')
    axes.SetYAxisLabelText('Y')
    axes.SetZAxisLabelText('Z')
    axes.SetTotalLength(1.0, 1.0, 1.0)
    axes.SetCylinderRadius(0.5 * axes.GetCylinderRadius())
    axes.SetConeRadius(1.025 * axes.GetConeRadius())
    axes.SetSphereRadius(1.5 * axes.GetSphereRadius())
    return axes


if __name__ == '__main__':
    main()
