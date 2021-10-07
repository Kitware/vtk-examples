#!/usr/bin/env python

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

    # Sphere.
    sphere_source = vtkSphereSource()
    sphere_source.SetCenter(-4.0, 0.0, 0.0)
    sphere_source.SetRadius(4.0)

    sphere_mapper = vtkPolyDataMapper()
    sphere_mapper.SetInputConnection(sphere_source.GetOutputPort())

    sphereActor = vtkActor()
    sphereActor.SetMapper(sphere_mapper)
    sphereActor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    # Regular Polygon.
    regular_polygon_source = vtkRegularPolygonSource()
    regular_polygon_source.SetCenter(4.0, 0.0, 0.0)
    regular_polygon_source.SetRadius(4.0)

    regular_polygon_mapper = vtkPolyDataMapper()
    regular_polygon_mapper.SetInputConnection(regular_polygon_source.GetOutputPort())

    regularPolygonActor = vtkActor()
    regularPolygonActor.SetMapper(regular_polygon_mapper)
    regularPolygonActor.GetProperty().SetColor(colors.GetColor3d('Cornsilk'))

    # A renderer and render window.
    ren = vtkRenderer()
    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(ren)
    ren_win.SetWindowName('BalloonWidget')

    # An interactor.
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    # Create the widget.
    balloonRep = vtkBalloonRepresentation()
    balloonRep.SetBalloonLayoutToImageRight()

    balloonWidget = vtkBalloonWidget()
    balloonWidget.SetInteractor(iren)
    balloonWidget.SetRepresentation(balloonRep)
    balloonWidget.AddBalloon(sphereActor, 'This is a sphere')
    balloonWidget.AddBalloon(regularPolygonActor, 'This is a regular polygon')

    # Add the actors to the scene.
    ren.AddActor(sphereActor)
    ren.AddActor(regularPolygonActor)
    ren.SetBackground(colors.GetColor3d('SlateGray'))

    # Render an image (lights and cameras are created automatically).
    ren_win.Render()
    balloonWidget.EnabledOn()

    # Begin mouse interaction.
    iren.Start()
    iren.Initialize()


if __name__ == '__main__':
    main()
