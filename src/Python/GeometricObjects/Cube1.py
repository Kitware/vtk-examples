#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkCubeSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create a rendering window and renderer.
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.SetWindowName('Cube1')
    renWin.AddRenderer(ren)

    # Create a renderwindowinteractor.
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Create cube.
    cube = vtkCubeSource()
    cube.Update()

    # mapper
    cubeMapper = vtkPolyDataMapper()
    cubeMapper.SetInputData(cube.GetOutput())

    # Actor.
    cubeActor = vtkActor()
    cubeActor.SetMapper(cubeMapper)
    cubeActor.GetProperty().SetColor(colors.GetColor3d('Banana'))

    # Assign actor to the renderer.
    ren.AddActor(cubeActor)

    ren.ResetCamera()
    ren.GetActiveCamera().Azimuth(30)
    ren.GetActiveCamera().Elevation(30)
    ren.ResetCameraClippingRange()
    ren.SetBackground(colors.GetColor3d('Silver'))

    # Enable user interface interactor.
    iren.Initialize()
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
