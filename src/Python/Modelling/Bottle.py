#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData
)
from vtkmodules.vtkFiltersCore import (
    vtkStripper,
    vtkTubeFilter
)
from vtkmodules.vtkFiltersModeling import vtkRotationalExtrusionFilter
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create the RenderWindow, Renderer and Interactor.
    #
    renderer = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(renderer)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Create the bottle profile.
    #
    points = vtkPoints()
    points.InsertPoint(0, 0.01, 0.0, 0.0)
    points.InsertPoint(1, 1.5, 0.0, 0.0)
    points.InsertPoint(2, 1.5, 0.0, 3.5)
    points.InsertPoint(3, 1.25, 0.0, 3.75)
    points.InsertPoint(4, 0.75, 0.0, 4.00)
    points.InsertPoint(5, 0.6, 0.0, 4.35)
    points.InsertPoint(6, 0.7, 0.0, 4.65)
    points.InsertPoint(7, 1.0, 0.0, 4.75)
    points.InsertPoint(8, 1.0, 0.0, 5.0)
    points.InsertPoint(9, 0.2, 0.0, 5.0)

    lines = vtkCellArray()
    lines.InsertNextCell(10)  # The number of points.
    lines.InsertCellPoint(0)
    lines.InsertCellPoint(1)
    lines.InsertCellPoint(2)
    lines.InsertCellPoint(3)
    lines.InsertCellPoint(4)
    lines.InsertCellPoint(5)
    lines.InsertCellPoint(6)
    lines.InsertCellPoint(7)
    lines.InsertCellPoint(8)
    lines.InsertCellPoint(9)

    profile = vtkPolyData()
    profile.SetPoints(points)
    profile.SetLines(lines)

    # Extrude the profile to make the bottle.
    #
    extrude = vtkRotationalExtrusionFilter()
    extrude.SetInputData(profile)
    extrude.SetResolution(60)

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(extrude.GetOutputPort())

    bottle = vtkActor()
    bottle.SetMapper(mapper)
    bottle.GetProperty().SetColor(colors.GetColor3d('Mint'))

    # Sisplay the profile.
    stripper = vtkStripper()
    stripper.SetInputData(profile)

    tubes = vtkTubeFilter()
    tubes.SetInputConnection(stripper.GetOutputPort())
    tubes.SetNumberOfSides(11)
    tubes.SetRadius(0.05)

    profileMapper = vtkPolyDataMapper()
    profileMapper.SetInputConnection(tubes.GetOutputPort())

    profileActor = vtkActor()
    profileActor.SetMapper(profileMapper)
    profileActor.GetProperty().SetColor(colors.GetColor3d('Tomato'))

    # Add the actors to the renderer, set the background and size.
    #
    renderer.AddActor(bottle)
    renderer.AddActor(profileActor)
    renderer.SetBackground(colors.GetColor3d('Burlywood'))

    renWin.SetSize(640, 480)
    renWin.SetWindowName('Bottle');
    renWin.Render()

    renderer.GetActiveCamera().SetPosition(1, 0, 0)
    renderer.GetActiveCamera().SetFocalPoint(0, 0, 0)
    renderer.GetActiveCamera().SetViewUp(0, 0, 1)
    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(30)
    renderer.GetActiveCamera().Elevation(30)

    # Render the image.
    #
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
