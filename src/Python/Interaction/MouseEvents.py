#!/usr/bin/env python
import vtk


class MyInteractorStyle(vtk.vtkInteractorStyleTrackballCamera):

    def __init__(self, parent=None):
        self.AddObserver('MiddleButtonPressEvent', self.middle_button_press_event)
        self.AddObserver('MiddleButtonReleaseEvent', self.middle_button_release_event)

    def middle_button_press_event(self, obj, event):
        print('Middle Button pressed')
        self.OnMiddleButtonDown()
        return

    def middle_button_release_event(self, obj, event):
        print('Middle Button released')
        self.OnMiddleButtonUp()
        return


def main():
    colors = vtk.vtkNamedColors()

    source = vtk.vtkSphereSource()
    source.SetCenter(0, 0, 0)
    source.SetRadius(1)
    source.Update()

    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(source.GetOutputPort())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    renderer = vtk.vtkRenderer()
    renderer.SetBackground(colors.GetColor3d('SlateGray'))
    renderer.AddActor(actor)

    renwin = vtk.vtkRenderWindow()
    renwin.AddRenderer(renderer)
    renwin.SetWindowName('MouseEvents')

    interactor = vtk.vtkRenderWindowInteractor()
    interactor.SetInteractorStyle(MyInteractorStyle())
    interactor.SetRenderWindow(renwin)

    interactor.Initialize()
    renwin.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
