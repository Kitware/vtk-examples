#!/usr/bin/env python
import vtk


def main():
    colors = vtk.vtkNamedColors()

    arrowSource = vtk.vtkArrowSource()

    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(arrowSource.GetOutputPort())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('PeachPuff'))

    # a renderer and render window
    renderer = vtk.vtkRenderer()
    renderWindow = vtk.vtkRenderWindow()
    renderWindow.AddRenderer(renderer)

    # an interactor
    renderWindowInteractor = vtk.vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Create the widget and its representation
    compassRepresentation = vtk.vtkCompassRepresentation()

    compassWidget = vtk.vtkCompassWidget()
    compassWidget.SetInteractor(renderWindowInteractor)
    compassWidget.SetRepresentation(compassRepresentation)

    # add the actors to the scene
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('MidnightBlue'))

    renderWindow.SetSize(640, 480)
    renderWindow.SetWindowName('CompassWidget')

    renderWindow.Render()
    compassWidget.EnabledOn()

    style = vtk.vtkInteractorStyleTrackballCamera()
    renderWindowInteractor.SetInteractorStyle(style)

    # begin interaction
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
