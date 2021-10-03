# !/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkPlane
from vtkmodules.vtkFiltersCore import vtkClipPolyData
from vtkmodules.vtkFiltersSources import vtkSuperquadricSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkProperty,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    # Create a superquadric
    superquadric_source = vtkSuperquadricSource()
    superquadric_source.SetPhiRoundness(3.1)
    superquadric_source.SetThetaRoundness(2.2)

    # Define a clipping plane
    clip_plane = vtkPlane()
    clip_plane.SetNormal(1.0, -1.0, -1.0)
    clip_plane.SetOrigin(0.0, 0.0, 0.0)

    # Clip the source with the plane
    clipper = vtkClipPolyData()
    clipper.SetInputConnection(superquadric_source.GetOutputPort())
    clipper.SetClipFunction(clip_plane)
    # This will give us the polygonal data that is clipped away
    clipper.GenerateClippedOutputOn()

    # Create a mapper and actor
    superquadric_mapper = vtkPolyDataMapper()
    superquadric_mapper.SetInputConnection(clipper.GetOutputPort())

    superquadric_actor = vtkActor()
    superquadric_actor.SetMapper(superquadric_mapper)

    colors = vtkNamedColors()

    # Create a property to be used for the back faces. Turn off all
    # shading by specifying 0 weights for specular and diffuse. Max the
    # ambient.
    back_faces = vtkProperty()
    back_faces.SetSpecular(0.0)
    back_faces.SetDiffuse(0.0)
    back_faces.SetAmbient(1.0)
    back_faces.SetAmbientColor(colors.GetColor3d('Tomato'))

    superquadric_actor.SetBackfaceProperty(back_faces)

    # Here we get the the polygonal data that is clipped away
    clipped_away_mapper = vtkPolyDataMapper()
    clipped_away_mapper.SetInputData(clipper.GetClippedOutput())
    clipped_away_mapper.ScalarVisibilityOff()

    # Let us display it as a faint object
    clipped_away_actor = vtkActor()
    clipped_away_actor.SetMapper(clipped_away_mapper)
    clipped_away_actor.GetProperty().SetDiffuseColor(colors.GetColor3d("Silver"))
    clipped_away_actor.GetProperty().SetOpacity(0.1)

    # Create a renderer
    renderer = vtkRenderer()
    renderer.SetBackground(colors.GetColor3d('SlateGray'))

    render_window = vtkRenderWindow()

    render_window.AddRenderer(renderer)

    render_window_interactor = vtkRenderWindowInteractor()
    render_window_interactor.SetRenderWindow(render_window)

    # Add the actor to the renderer
    renderer.AddActor(superquadric_actor)
    renderer.AddActor(clipped_away_actor)
    render_window.SetSize(600, 600)
    renderer.ResetCamera()
    renderer.GetActiveCamera().Dolly(1.5)
    renderer.ResetCameraClippingRange()
    render_window.Render()
    render_window.SetWindowName('SolidClip')

    # Interact with the window
    render_window_interactor.Start()


if __name__ == '__main__':
    main()
