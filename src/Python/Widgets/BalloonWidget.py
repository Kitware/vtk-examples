#!/usr/bin/env python

# Needs fixing

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import (
    vtkRegularPolygonSource,
    vtkSphereSource
)
from vtkmodules.vtkInteractionWidgets import (
    vtkBalloonRepresentation,
    vtkBalloonWidget
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Sphere
    sphereSource = vtkSphereSource()
    sphereSource.SetCenter(-4.0, 0.0, 0.0)
    sphereSource.SetRadius(4.0)

    sphereMapper = vtkPolyDataMapper()
    sphereMapper.SetInputConnection(sphereSource.GetOutputPort())

    sphereActor = vtkActor()
    sphereActor.SetMapper(sphereMapper)
    sphereActor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    # Regular Polygon
    regularPolygonSource = vtkRegularPolygonSource()
    regularPolygonSource.SetCenter(4.0, 0.0, 0.0)
    regularPolygonSource.SetRadius(4.0)

    regularPolygonMapper = vtkPolyDataMapper()
    regularPolygonMapper.SetInputConnection(regularPolygonSource.GetOutputPort())

    regularPolygonActor = vtkActor()
    regularPolygonActor.SetMapper(regularPolygonMapper)
    regularPolygonActor.GetProperty().SetColor(colors.GetColor3d('Cornsilk'))

    # A renderer and render window
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('BalloonWidget')

    # An interactor
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Create the widget
    balloonRep = vtkBalloonRepresentation()
    balloonRep.SetBalloonLayoutToImageRight()

    balloonWidget = vtkBalloonWidget()
    balloonWidget.SetInteractor(renderWindowInteractor)
    balloonWidget.SetRepresentation(balloonRep)
    balloonWidget.AddBalloon(sphereActor, 'This is a sphere')
    balloonWidget.AddBalloon(regularPolygonActor, 'This is a regular polygon')

    # Add the actors to the scene
    renderer.AddActor(sphereActor)
    renderer.AddActor(regularPolygonActor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))

    # Render an image (lights and cameras are created automatically)
    renderWindow.Render()
    balloonWidget.EnabledOn()

    # Begin mouse interaction
    renderWindowInteractor.Start()
    renderWindowInteractor.Initialize()


if __name__ == '__main__':
    main()
