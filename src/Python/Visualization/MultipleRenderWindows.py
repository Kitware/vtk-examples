#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import (
    vtkConeSource,
    vtkCubeSource,
    vtkCylinderSource,
    vtkSphereSource
)
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def get_program_parameters():
    import argparse
    description = 'Display multiple render windows.'
    epilogue = '''
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-s', default=False, action='store_true',
                        help='Simultaneous camera position updating.')
    args = parser.parse_args()
    return args.s


def main():
    simultaneous_update = get_program_parameters()

    colors = vtkNamedColors()
    # Have some fun with colors
    ren_bkg = ['AliceBlue', 'GhostWhite', 'WhiteSmoke', 'Seashell']
    actor_color = ['Bisque', 'RosyBrown', 'Goldenrod', 'Chocolate']

    # Window sizes and spacing.
    width = 300
    height = 300
    # Add extra space around each window.
    dx = 20
    dy = 40
    w = width + dx
    h = height + dy

    interactors = list()
    running = [True, True, True, True]

    camera = None
    sources = get_sources()

    kpis = list()
    for i in range(0, 4):
        ren_win = vtkRenderWindow()
        ren_win.SetSize(width, height)

        renderer = vtkRenderer()

        # Share the camera between viewports.
        if i == 0:
            camera = renderer.GetActiveCamera()
            camera.Azimuth(30)
            camera.Elevation(30)
        else:
            renderer.SetActiveCamera(camera)

        ren_win.AddRenderer(renderer)

        iren = vtkRenderWindowInteractor()

        interactors.append(iren)

        iren.SetRenderWindow(ren_win)
        ren_win.Render()
        ren_win.SetWindowName('MultipleRenderWindows {:d}'.format(i))
        ren_win.SetPosition((i % 2) * w, h - (i // 2) * h)

        # Create a mapper and actor.
        mapper = vtkPolyDataMapper()
        mapper.SetInputConnection(sources[i].GetOutputPort())

        actor = vtkActor()
        actor.SetMapper(mapper)
        actor.GetProperty().SetColor(colors.GetColor3d(actor_color[i]))

        renderer.AddActor(actor)
        renderer.SetBackground(colors.GetColor3d(ren_bkg[i]))

        renderer.ResetCamera()

        running[i] = True
        kpis.append(KeyPressInteractorStyle(parent=iren))
        interactors[i].SetInteractorStyle(kpis[i])
        kpis[i].status = running[i]

    if simultaneous_update:
        interactors[0].Initialize()
        while all(x is True for x in running):
            for i in range(0, 4):
                running[i] = kpis[i].status
                if running[i]:
                    interactors[i].ProcessEvents()
                    interactors[i].Render()
                else:
                    interactors[i].TerminateApp()
                    print('Window', i, 'has stopped running.')
    else:
        interactors[0].Start()


def get_sources():
    sources = list()

    # Create a sphere
    sphere = vtkSphereSource()
    sphere.SetCenter(0.0, 0.0, 0.0)
    sphere.Update()
    sources.append(sphere)
    # Create a cone
    cone = vtkConeSource()
    cone.SetCenter(0.0, 0.0, 0.0)
    cone.SetDirection(0, 1, 0)
    cone.Update()
    sources.append(cone)
    # Create a cube
    cube = vtkCubeSource()
    cube.SetCenter(0.0, 0.0, 0.0)
    cube.Update()
    sources.append(cube)
    # Create a cylinder
    cylinder = vtkCylinderSource()
    cylinder.SetCenter(0.0, 0.0, 0.0)
    cylinder.Update()
    sources.append(cylinder)

    return sources


class KeyPressInteractorStyle(vtkInteractorStyleTrackballCamera):

    def __init__(self, parent=None, status=True):
        self.parent = vtkRenderWindowInteractor()
        self.status = status
        if parent is not None:
            self.parent = parent

        self.AddObserver('KeyPressEvent', self.key_press_event)

    def key_press_event(self, obj, event):
        key = self.parent.GetKeySym().lower()
        if key == 'e' or key == 'q':
            self.status = False
        return


if __name__ == '__main__':
    main()
