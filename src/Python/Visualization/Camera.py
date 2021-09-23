from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)
# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2


def main():
    colors = vtkNamedColors()

    # Create a sphere
    sphereSource = vtkSphereSource()
    sphereSource.SetCenter(0.0, 0.0, 0.0)
    sphereSource.SetRadius(10)
    sphereSource.SetPhiResolution(30)
    sphereSource.SetThetaResolution(30)
    sphereSource.Update()

    # Create a mapper and actor
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(sphereSource.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetSpecular(0.6)
    actor.GetProperty().SetSpecularPower(30)
    actor.GetProperty().SetColor(colors.GetColor3d('LightSkyBlue'))

    camera = vtkCamera()
    camera.SetPosition(0, 0, 100)
    camera.SetFocalPoint(0, 0, 0)

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    renderer.SetActiveCamera(camera)

    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('Camera')

    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actor to the scene
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('MistyRose'))

    # Render and interact
    renderWindowInteractor.Initialize()
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
