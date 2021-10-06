#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkArrowSource
from vtkmodules.vtkGeovisCore import (
    vtkCompassRepresentation,
    vtkCompassWidget
)
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    arrowSource = vtkArrowSource()

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(arrowSource.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('PeachPuff'))

    # a renderer and render window
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)

    # an interactor
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Create the widget and its representation
    compassRepresentation = vtkCompassRepresentation()

    compassWidget = vtkCompassWidget()
    compassWidget.SetInteractor(renderWindowInteractor)
    compassWidget.SetRepresentation(compassRepresentation)

    # add the actors to the scene
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('MidnightBlue'))

    renderWindow.SetSize(640, 480)
    renderWindow.SetWindowName('CompassWidget')

    renderWindow.Render()
    compassWidget.EnabledOn()

    style = vtkInteractorStyleTrackballCamera()
    renderWindowInteractor.SetInteractorStyle(style)

    # begin interaction
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
