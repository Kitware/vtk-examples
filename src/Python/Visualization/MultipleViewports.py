#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import (
    vtkConeSource,
    vtkCubeSource,
    vtkCylinderSource,
    vtkSphereSource
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

    # One render window, multiple viewports.
    rw = vtkRenderWindow()
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(rw)

    # Define viewport ranges.
    xmins = [0, .5, 0, .5]
    xmaxs = [0.5, 1, 0.5, 1]
    ymins = [0, 0, .5, .5]
    ymaxs = [0.5, 0.5, 1, 1]

    # Have some fun with colors.
    ren_bkg = ['AliceBlue', 'GhostWhite', 'WhiteSmoke', 'Seashell']
    actor_color = ['Bisque', 'RosyBrown', 'Goldenrod', 'Chocolate']

    sources = get_sources()
    for i in range(4):
        ren = vtkRenderer()
        rw.AddRenderer(ren)
        ren.SetViewport(xmins[i], ymins[i], xmaxs[i], ymaxs[i])

        # Share the camera between viewports.
        if i == 0:
            camera = ren.GetActiveCamera()
            camera.Azimuth(30)
            camera.Elevation(30)
        else:
            ren.SetActiveCamera(camera)

        # Create a mapper and actor
        mapper = vtkPolyDataMapper()
        mapper.SetInputConnection(sources[i].GetOutputPort())
        actor = vtkActor()
        actor.GetProperty().SetColor(colors.GetColor3d(actor_color[i]))
        actor.SetMapper(mapper)
        ren.AddActor(actor)
        ren.SetBackground(colors.GetColor3d(ren_bkg[i]))

        ren.ResetCamera()

    rw.Render()
    rw.SetWindowName('MultipleViewPorts')
    rw.SetSize(600, 600)
    iren.Start()


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


if __name__ == '__main__':
    main()
