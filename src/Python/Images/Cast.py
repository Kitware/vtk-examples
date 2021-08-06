#!/usr/bin/env python

import vtk


def main():
    colors = vtk.vtkNamedColors()

    # Create a float image
    source = vtk.vtkImageMandelbrotSource()
    source.Update()

    print(source.GetOutput().GetScalarTypeAsString())

    castFilter = vtk.vtkImageCast()
    castFilter.SetInputConnection(source.GetOutputPort())
    castFilter.SetOutputScalarTypeToUnsignedChar()
    castFilter.Update()

    # Create an actor
    actor = vtk.vtkImageActor()
    actor.GetMapper().SetInputConnection(castFilter.GetOutputPort())

    # Setup renderer
    renderer = vtk.vtkRenderer()
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('DarkSlateGray'))
    renderer.ResetCamera()

    # Setup render window
    renderWindow = vtk.vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('Cast')

    # Setup render window interactor
    renderWindowInteractor = vtk.vtkRenderWindowInteractor()
    style = vtk.vtkInteractorStyleImage()
    renderWindowInteractor.SetInteractorStyle(style)

    # Render and start interaction
    renderWindowInteractor.SetRenderWindow(renderWindow)
    renderWindow.Render()
    renderWindowInteractor.Initialize()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
