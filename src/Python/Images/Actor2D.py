#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonDataModel import vtkPolyData
from vtkmodules.vtkFiltersGeneral import vtkVertexGlyphFilter
from vtkmodules.vtkRenderingCore import (
    vtkActor2D,
    vtkPolyDataMapper2D,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()
    points = vtkPoints()
    points.InsertNextPoint(10, 10, 0)
    points.InsertNextPoint(100, 100, 0)
    points.InsertNextPoint(200, 200, 0)

    polydata = vtkPolyData()
    polydata.SetPoints(points)

    glyphFilter = vtkVertexGlyphFilter()
    glyphFilter.SetInputData(polydata)
    glyphFilter.Update()

    mapper = vtkPolyDataMapper2D()
    mapper.SetInputConnection(glyphFilter.GetOutputPort())
    mapper.Update()

    actor = vtkActor2D()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('Gold'))
    actor.GetProperty().SetPointSize(8)

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actor to the scene
    renderer.AddActor(actor)
    renderWindow.SetSize(300, 300)
    renderer.SetBackground(colors.GetColor3d('DarkSlateGray'))

    renderWindow.SetWindowName('Actor2D')

    # Render and interact
    renderWindow.Render()
    # w2if = vtkWindowToImageFilter()
    # w2if.SetInput(renderWindow)
    # w2if.Update()
    #
    # writer = vtkPNGWriter()
    # writer.SetFileName('TestActor2D.png')
    # writer.SetInputConnection(w2if.GetOutputPort())
    # writer.Write()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
