#!/usr/bin/env python3

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingFreeType
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
# noinspection PyUnresolvedReferences
from vtkmodules.vtkCommonCore import vtkCommand
from vtkmodules.vtkCommonCore import (
    vtkMinimalStandardRandomSequence
)
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkBillboardTextActor3D,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # For testing
    rng = vtkMinimalStandardRandomSequence()
    # rng.SetSeed(8775070)
    rng.SetSeed(5127)

    # Create a renderer
    renderer = vtkRenderer()
    renderer.SetBackground(colors.GetColor3d('DarkSlateGray'))

    # Create a render window
    render_window = vtkRenderWindow()
    render_window.AddRenderer(renderer)
    render_window.SetWindowName('BillboardTextActor3D')

    # Create an interactor
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(render_window)

    # Create a sphere
    sphere_source = vtkSphereSource()
    sphere_source.SetCenter(0.0, 0.0, 0.0)
    sphere_source.SetRadius(1.0)

    min_r = -10.0
    max_r = 10.0

    for i in range(0, 10):
        if i == 0:
            # Create an actor representing the origin
            mapper = vtkPolyDataMapper()
            mapper.SetInputConnection(sphere_source.GetOutputPort())

            actor = vtkActor()
            actor.SetMapper(mapper)
            actor.SetPosition(0, 0, 0)
            actor.GetProperty().SetColor(colors.GetColor3d('Peacock'))

            renderer.AddActor(actor)

        # Create a mapper
        mapper = vtkPolyDataMapper()
        mapper.SetInputConnection(sphere_source.GetOutputPort())

        # Create an actor
        actor = vtkActor()
        actor.SetMapper(mapper)
        actor.SetPosition(0, 0, 0)
        actor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

        # Set up the text and add it to the renderer
        text_actor = vtkBillboardTextActor3D()
        text_actor.SetInput('')
        text_actor.SetPosition(actor.GetPosition())
        text_actor.GetTextProperty().SetFontSize(12)
        text_actor.GetTextProperty().SetColor(colors.GetColor3d('Gold'))
        text_actor.GetTextProperty().SetJustificationToCentered()

        position = random_position(min_r, max_r, rng)
        actor.SetPosition(position)
        # If you want to use a callback, do this:
        # observer = ActorCallback(text_actor)
        # actor.AddObserver(vtkCommand.NoEvent, observer(actor))
        # Otherwise do this:
        label = f'{position[0]:0.3g}, {position[1]:0.3g}, {position[2]:0.3g}'
        text_actor.SetPosition(position)
        text_actor.SetInput(label)

        renderer.AddActor(actor)
        renderer.AddActor(text_actor)

    render_window.Render()
    render_window.SetWindowName('BillboardTextActor3D')
    iren.Start()


class ActorCallback:
    def __init__(self, text_actor):
        self.text_actor = text_actor

    def __call__(self, caller):
        position = caller.GetPosition()
        label = f'{position[0]:0.3g}, {position[1]:0.3g}, {position[2]:0.3g}'
        self.text_actor.SetPosition(position)
        self.text_actor.SetInput(label)


def random_position(min_r, max_r, rng):
    p = list()
    for i in range(0, 3):
        p.append(rng.GetRangeValue(min_r, max_r))
        rng.Next()
    return p


if __name__ == '__main__':
    main()
