#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    source = vtkSphereSource()
    source.SetCenter(0, 0, 0)
    source.SetRadius(1)
    source.Update()

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(source.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    renderer = vtkRenderer()
    renderer.SetBackground(colors.GetColor3d('SlateGray'))
    renderer.AddActor(actor)

    renwin = vtkRenderWindow()
    renwin.AddRenderer(renderer)
    renwin.SetWindowName('MouseEventsObserver')

    interactor = vtkRenderWindowInteractor()
    interactor.SetInteractorStyle(vtkInteractorStyleTrackballCamera())
    interactor.SetRenderWindow(renwin)

    def DummyFunc1(obj, ev):
        print('Before Event')

    def DummyFunc2(obj, ev):
        print('After Event')

    # Print interator gives you a list of registered observers of the current
    # interactor style
    # print(interactor)

    # adding priorities allow to control the order of observer execution
    # (highest value first! if equal the first added observer is called first)
    interactor.RemoveObservers('LeftButtonPressEvent')
    interactor.AddObserver('LeftButtonPressEvent', DummyFunc1, 1.0)
    interactor.AddObserver('LeftButtonPressEvent', DummyFunc2, -1.0)
    interactor.Initialize()
    renwin.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
