#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkMinimalStandardRandomSequence
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkPropPicker,
    vtkProperty,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)

colors = vtkNamedColors()
NUMBER_OF_SPHERES = 10


class MouseInteractorHighLightActor(vtkInteractorStyleTrackballCamera):

    def __init__(self, parent=None):
        self.AddObserver("LeftButtonPressEvent", self.leftButtonPressEvent)

        self.LastPickedActor = None
        self.LastPickedProperty = vtkProperty()

    def leftButtonPressEvent(self, obj, event):
        clickPos = self.GetInteractor().GetEventPosition()

        picker = vtkPropPicker()
        picker.Pick(clickPos[0], clickPos[1], 0, self.GetDefaultRenderer())

        # get the new
        self.NewPickedActor = picker.GetActor()

        # If something was selected
        if self.NewPickedActor:
            # If we picked something before, reset its property
            if self.LastPickedActor:
                self.LastPickedActor.GetProperty().DeepCopy(self.LastPickedProperty)

            # Save the property of the picked actor so that we can
            # restore it next time
            self.LastPickedProperty.DeepCopy(self.NewPickedActor.GetProperty())
            # Highlight the picked actor by changing its properties
            self.NewPickedActor.GetProperty().SetColor(colors.GetColor3d('Red'))
            self.NewPickedActor.GetProperty().SetDiffuse(1.0)
            self.NewPickedActor.GetProperty().SetSpecular(0.0)
            self.NewPickedActor.GetProperty().EdgeVisibilityOn()

            # save the last picked actor
            self.LastPickedActor = self.NewPickedActor

        self.OnLeftButtonDown()
        return


def main():
    # A renderer and render window
    renderer = vtkRenderer()
    renderer.SetBackground(colors.GetColor3d('SteelBlue'))

    renwin = vtkRenderWindow()
    renwin.AddRenderer(renderer)
    renwin.SetSize(640, 480)
    renwin.SetWindowName('HighlightPickedActor')

    # An interactor
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renwin)

    # add the custom style
    style = MouseInteractorHighLightActor()
    style.SetDefaultRenderer(renderer)
    interactor.SetInteractorStyle(style)

    randomSequence = vtkMinimalStandardRandomSequence()
    # randomSequence.SetSeed(1043618065)
    # randomSequence.SetSeed(5170)
    randomSequence.SetSeed(8775070)
    # Add spheres to play with
    for i in range(NUMBER_OF_SPHERES):
        source = vtkSphereSource()

        # random position and radius
        x = randomSequence.GetRangeValue(-5.0, 5.0)
        randomSequence.Next()
        y = randomSequence.GetRangeValue(-5.0, 5.0)
        randomSequence.Next()
        z = randomSequence.GetRangeValue(-5.0, 5.0)
        randomSequence.Next()
        radius = randomSequence.GetRangeValue(0.5, 1.0)
        randomSequence.Next()

        source.SetRadius(radius)
        source.SetCenter(x, y, z)
        source.SetPhiResolution(11)
        source.SetThetaResolution(21)

        mapper = vtkPolyDataMapper()
        mapper.SetInputConnection(source.GetOutputPort())
        actor = vtkActor()
        actor.SetMapper(mapper)

        r = randomSequence.GetRangeValue(0.4, 1.0)
        randomSequence.Next()
        g = randomSequence.GetRangeValue(0.4, 1.0)
        randomSequence.Next()
        b = randomSequence.GetRangeValue(0.4, 1.0)
        randomSequence.Next()

        actor.GetProperty().SetDiffuseColor(r, g, b)
        actor.GetProperty().SetDiffuse(.8)
        actor.GetProperty().SetSpecular(.5)
        actor.GetProperty().SetSpecularColor(colors.GetColor3d('White'))
        actor.GetProperty().SetSpecularPower(30.0)

        renderer.AddActor(actor)

    # Start
    interactor.Initialize()
    renwin.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
