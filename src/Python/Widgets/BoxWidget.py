#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersSources import vtkConeSource
from vtkmodules.vtkInteractionWidgets import vtkBoxWidget
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


# Call back function to resize the cone


def boxCallback(obj, event):
    t = vtkTransform()
    obj.GetTransform(t)
    obj.GetProp3D().SetUserTransform(t)


def main():
    colors = vtkNamedColors()

    # Create a Cone
    cone = vtkConeSource()
    cone.SetResolution(20)
    coneMapper = vtkPolyDataMapper()
    coneMapper.SetInputConnection(cone.GetOutputPort())
    coneActor = vtkActor()
    coneActor.SetMapper(coneMapper)
    coneActor.GetProperty().SetColor(colors.GetColor3d('BurlyWood'))

    # A renderer and render window
    renderer = vtkRenderer()
    renderer.SetBackground(colors.GetColor3d('Blue'))
    renderer.AddActor(coneActor)

    renwin = vtkRenderWindow()
    renwin.AddRenderer(renderer)
    renwin.SetWindowName('BoxWidget')

    # An interactor
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renwin)

    # A Box widget
    boxWidget = vtkBoxWidget()
    boxWidget.SetInteractor(interactor)
    boxWidget.SetProp3D(coneActor)
    boxWidget.SetPlaceFactor(1.25)  # Make the box 1.25x larger than the actor
    boxWidget.PlaceWidget()
    boxWidget.On()

    # Connect the event to a function
    boxWidget.AddObserver('InteractionEvent', boxCallback)

    # Start
    interactor.Initialize()
    renwin.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
