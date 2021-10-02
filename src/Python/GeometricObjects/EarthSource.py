#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersHybrid import vtkEarthSource
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Earth source
    earthSource = vtkEarthSource()
    earthSource.OutlineOn()
    earthSource.Update()
    r = earthSource.GetRadius()

    # Create a sphere
    sphere = vtkSphereSource()
    sphere.SetThetaResolution(100)
    sphere.SetPhiResolution(100)
    sphere.SetRadius(earthSource.GetRadius())

    # Create a mapper and actor
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(earthSource.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('Black'))

    sphereMapper = vtkPolyDataMapper()
    sphereMapper.SetInputConnection(sphere.GetOutputPort())

    sphereActor = vtkActor()
    sphereActor.SetMapper(sphereMapper)
    sphereActor.GetProperty().SetColor(colors.GetColor3d('PeachPuff'))

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actor to the scene
    renderer.AddActor(actor)
    renderer.AddActor(sphereActor)
    renderer.SetBackground(colors.GetColor3d('Black'))

    renderWindow.SetSize(640, 480)
    renderWindow.SetWindowName('EarthSource')

    # Render and interact
    renderWindow.Render()

    # # screenshot code:
    # w2if = vtkWindowToImageFilter()
    # w2if.SetInput(renderWindow)
    # w2if.Update()
    #
    # writer = vtkPNGWriter()
    # writer.SetFileName('TestEarthSource.png')
    # writer.SetInputConnection(w2if.GetOutputPort())
    # writer.Write()

    # begin interaction
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
