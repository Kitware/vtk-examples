#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# by Panos Mavrogiorgos, email : pmav99 >a< gmail

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkInteractionWidgets import (
    vtkTextRepresentation,
    vtkTextWidget
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTextActor
)


def main():
    colors = vtkNamedColors()

    # colors.SetColor('bkg', [0.1, 0.2, 0.4, 1.0])

    source = vtkSphereSource()

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(source.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    renderer = vtkRenderer()
    render_window = vtkRenderWindow()
    render_window.AddRenderer(renderer)

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(render_window)

    # Create the TextActor
    text_actor = vtkTextActor()
    text_actor.SetInput('This is a test')
    text_actor.GetTextProperty().SetColor(colors.GetColor3d('Lime'))

    # Create the text representation. Used for positioning the text_actor
    text_representation = vtkTextRepresentation()
    text_representation.GetPositionCoordinate().SetValue(0.15, 0.15)
    text_representation.GetPosition2Coordinate().SetValue(0.7, 0.2)

    # Create the TextWidget
    # Note that the SelectableOff method MUST be invoked!
    # According to the documentation :
    #
    # SelectableOn/Off indicates whether the interior region of the widget can be
    # selected or not. If not, then events (such as left mouse down) allow the user
    # to 'move' the widget, and no selection is possible. Otherwise the
    # SelectRegion() method is invoked.
    text_widget = vtkTextWidget()
    text_widget.SetRepresentation(text_representation)

    text_widget.SetInteractor(interactor)
    text_widget.SetTextActor(text_actor)
    text_widget.SelectableOff()

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('MidnightBLue'))
    render_window.SetSize(300, 300)
    render_window.SetWindowName('TextWidget')

    interactor.Initialize()
    render_window.Render()
    text_widget.On()
    interactor.Start()


if __name__ == '__main__':
    main()
