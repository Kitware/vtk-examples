#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersGeneral import vtkAxes
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkFollower,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)
from vtkmodules.vtkRenderingFreeType import vtkVectorText


def main():
    colors = vtkNamedColors()

    # Create the axes and the associated mapper and actor.
    axes = vtkAxes()
    axes.SetOrigin(0, 0, 0)
    axesMapper = vtkPolyDataMapper()
    axesMapper.SetInputConnection(axes.GetOutputPort())
    axesActor = vtkActor()
    axesActor.SetMapper(axesMapper)

    # Create the 3D text and the associated mapper and follower (a type of actor).  Position the text so it is displayed over the origin of the axes.
    atext = vtkVectorText()
    atext.SetText('Origin')
    textMapper = vtkPolyDataMapper()
    textMapper.SetInputConnection(atext.GetOutputPort())
    textActor = vtkFollower()
    textActor.SetMapper(textMapper)
    textActor.SetScale(0.2, 0.2, 0.2)
    textActor.AddPosition(0, -0.1, 0)
    textActor.GetProperty().SetColor(colors.GetColor3d('Peacock'))

    # Create the Renderer, RenderWindow, and RenderWindowInteractor.
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetSize(640, 480)

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    style = vtkInteractorStyleTrackballCamera()
    interactor.SetInteractorStyle(style)

    # Add the actors to the renderer.
    renderer.AddActor(axesActor)
    renderer.AddActor(textActor)

    renderer.SetBackground(colors.GetColor3d('Silver'))

    # Zoom in closer.
    renderer.ResetCamera()
    renderer.GetActiveCamera().Zoom(1.6)

    renderer.SetBackground(colors.GetColor3d('Silver'))

    # Reset the clipping range of the camera; set the camera of the follower; render.
    renderer.ResetCameraClippingRange()
    textActor.SetCamera(renderer.GetActiveCamera())

    interactor.Initialize()
    renderWindow.SetWindowName('TextOrigin')
    renderWindow.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
