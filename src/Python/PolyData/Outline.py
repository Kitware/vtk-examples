#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersModeling import vtkOutlineFilter
from vtkmodules.vtkFiltersSources import vtkConeSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # create a rendering window and renderer
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.SetWindowName('Outline')

    renWin.AddRenderer(ren)

    # create a renderwindowinteractor
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # create source
    source = vtkConeSource()
    source.SetCenter(0, 0, 0)
    source.SetResolution(100)

    # mapper
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(source.GetOutputPort())

    # actor
    actor1 = vtkActor()
    actor1.SetMapper(mapper)
    actor1.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    # outline
    outline = vtkOutlineFilter()
    outline.SetInputConnection(source.GetOutputPort())
    mapper2 = vtkPolyDataMapper()
    mapper2.SetInputConnection(outline.GetOutputPort())

    actor2 = vtkActor()
    actor2.SetMapper(mapper2)
    actor2.GetProperty().SetColor(colors.GetColor3d('Gold'))

    # assign actor to the renderer
    ren.AddActor(actor1)
    ren.AddActor(actor2)
    ren.SetBackground(colors.GetColor3d('MidnightBlue'))

    # enable user interface interactor
    iren.Initialize()
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
