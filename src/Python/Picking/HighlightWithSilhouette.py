# !/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkMinimalStandardRandomSequence
from vtkmodules.vtkFiltersHybrid import vtkPolyDataSilhouette
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkPropPicker,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def get_program_parameters():
    import argparse
    description = 'Highlighting a selected object with a silhouette.'
    epilogue = '''
Click on the object to highlight it.
The selected object is highlighted with a silhouette.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('numberOfSpheres', nargs='?', type=int, default=10,
                        help='The number of spheres, default is 10.')
    args = parser.parse_args()
    return args.numberOfSpheres


class MouseInteractorHighLightActor(vtkInteractorStyleTrackballCamera):

    def __init__(self, silhouette=None, silhouetteActor=None):
        self.AddObserver("LeftButtonPressEvent", self.onLeftButtonDown)
        self.LastPickedActor = None
        self.Silhouette = silhouette
        self.SilhouetteActor = silhouetteActor

    def onLeftButtonDown(self, obj, event):
        clickPos = self.GetInteractor().GetEventPosition()

        #  Pick from this location.
        picker = vtkPropPicker()
        picker.Pick(clickPos[0], clickPos[1], 0, self.GetDefaultRenderer())
        self.LastPickedActor = picker.GetActor()

        # If we picked something before, remove the silhouette actor and
        # generate a new one.
        if self.LastPickedActor:
            self.GetDefaultRenderer().RemoveActor(self.SilhouetteActor)

            # Highlight the picked actor by generating a silhouette
            self.Silhouette.SetInputData(self.LastPickedActor.GetMapper().GetInput())
            self.GetDefaultRenderer().AddActor(self.SilhouetteActor)

        #  Forward events
        self.OnLeftButtonDown()
        return

    def SetSilhouette(self, silhouette):
        self.Silhouette = silhouette

    def SetSilhouetteActor(self, silhouetteActor):
        self.SilhouetteActor = silhouetteActor


def main():
    numberOfSpheres = get_program_parameters()
    colors = vtkNamedColors()

    # A renderer and render window
    renderer = vtkRenderer()
    renderer.SetBackground(colors.GetColor3d('SteelBlue'))

    renderWindow = vtkRenderWindow()
    renderWindow.SetSize(640, 480)
    renderWindow.AddRenderer(renderer)

    # An interactor
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    randomSequence = vtkMinimalStandardRandomSequence()
    # randomSequence.SetSeed(1043618065)
    # randomSequence.SetSeed(5170)
    randomSequence.SetSeed(8775070)
    # Add spheres to play with
    for i in range(numberOfSpheres):
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
        actor.GetProperty().SetDiffuse(0.8)
        actor.GetProperty().SetSpecular(0.5)
        actor.GetProperty().SetSpecularColor(colors.GetColor3d('White'))
        actor.GetProperty().SetSpecularPower(30.0)

        renderer.AddActor(actor)

    # Render and interact
    renderWindow.Render()

    # Create the silhouette pipeline, the input data will be set in the
    # interactor
    silhouette = vtkPolyDataSilhouette()
    silhouette.SetCamera(renderer.GetActiveCamera())

    # Create mapper and actor for silhouette
    silhouetteMapper = vtkPolyDataMapper()
    silhouetteMapper.SetInputConnection(silhouette.GetOutputPort())

    silhouetteActor = vtkActor()
    silhouetteActor.SetMapper(silhouetteMapper)
    silhouetteActor.GetProperty().SetColor(colors.GetColor3d("Tomato"))
    silhouetteActor.GetProperty().SetLineWidth(5)

    # Set the custom type to use for interaction.
    style = MouseInteractorHighLightActor(silhouette, silhouetteActor)
    style.SetDefaultRenderer(renderer)

    # Start
    interactor.Initialize()
    interactor.SetInteractorStyle(style)
    renderWindow.SetWindowName('HighlightWithSilhouette')
    renderWindow.Render()

    interactor.Start()


if __name__ == "__main__":
    main()
