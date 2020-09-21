#!/usr/bin/env python
import vtk


def main():
    colors = vtk.vtkNamedColors()

    # Earth source
    earthSource = vtk.vtkEarthSource()
    earthSource.OutlineOn()
    earthSource.Update()
    r = earthSource.GetRadius()

    # Create a sphere
    sphere = vtk.vtkSphereSource()
    sphere.SetThetaResolution(100)
    sphere.SetPhiResolution(100)
    sphere.SetRadius(earthSource.GetRadius())

    # Create a mapper and actor
    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(earthSource.GetOutputPort())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('Black'))

    sphereMapper = vtk.vtkPolyDataMapper()
    sphereMapper.SetInputConnection(sphere.GetOutputPort())

    sphereActor = vtk.vtkActor()
    sphereActor.SetMapper(sphereMapper)
    sphereActor.GetProperty().SetColor(colors.GetColor3d('PeachPuff'))

    # Create a renderer, render window, and interactor
    renderer = vtk.vtkRenderer()
    renderWindow = vtk.vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtk.vtkRenderWindowInteractor()
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
    # w2if = vtk.vtkWindowToImageFilter()
    # w2if.SetInput(renderWindow)
    # w2if.Update()
    #
    # writer = vtk.vtkPNGWriter()
    # writer.SetFileName('TestEarthSource.png')
    # writer.SetInputConnection(w2if.GetOutputPort())
    # writer.Write()

    # begin interaction
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
