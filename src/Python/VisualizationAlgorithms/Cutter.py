#!/usr/bin/env python

# A simple script to demonstrate the vtkCutter function

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkPlane
from vtkmodules.vtkFiltersCore import vtkCutter
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

    # Create a cube
    cube = vtkCubeSource()
    cube.SetXLength(40)
    cube.SetYLength(30)
    cube.SetZLength(20)
    cubeMapper = vtkPolyDataMapper()
    cubeMapper.SetInputConnection(cube.GetOutputPort())

    # create a plane to cut,here it cuts in the XZ direction (xz normal=(1,0,0);XY =(0,0,1),YZ =(0,1,0)
    plane = vtkPlane()
    plane.SetOrigin(10, 0, 0)
    plane.SetNormal(1, 0, 0)

    # create cutter
    cutter = vtkCutter()
    cutter.SetCutFunction(plane)
    cutter.SetInputConnection(cube.GetOutputPort())
    cutter.Update()
    cutterMapper = vtkPolyDataMapper()
    cutterMapper.SetInputConnection(cutter.GetOutputPort())

    # create plane actor
    planeActor = vtkActor()
    planeActor.GetProperty().SetColor(colors.GetColor3d('Yellow'))
    planeActor.GetProperty().SetLineWidth(2)
    planeActor.GetProperty().SetAmbient(1.0)
    planeActor.GetProperty().SetDiffuse(0.0)
    planeActor.SetMapper(cutterMapper)

    # create cube actor
    cubeActor = vtkActor()
    cubeActor.GetProperty().SetColor(colors.GetColor3d('Aquamarine'))
    cubeActor.GetProperty().SetOpacity(0.5)
    cubeActor.SetMapper(cubeMapper)

    # create renderers and add actors of plane and cube
    ren = vtkRenderer()
    ren.AddActor(planeActor)
    ren.AddActor(cubeActor)
    ren.SetBackground(colors.GetColor3d('Silver'))

    # Add renderer to renderwindow and render
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)
    renWin.SetSize(600, 600)
    renWin.SetWindowName('Cutter')
    renWin.Render()

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    camera = ren.GetActiveCamera()
    camera.SetPosition(-37.2611, -86.2155, 44.841)
    camera.SetFocalPoint(0.569422, -1.65124, -2.49482)
    camera.SetViewUp(0.160129, 0.42663, 0.890138)
    camera.SetDistance(104.033)
    camera.SetClippingRange(55.2019, 165.753)

    renWin.Render()

    iren.Start()


if __name__ == '__main__':
    main()
