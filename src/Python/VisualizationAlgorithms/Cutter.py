#!/usr/bin/env python

# A simple script to demonstrate the vtkCutter function

import vtkmodules.all as vtk


def main():
    colors = vtk.vtkNamedColors()

    # Create a cube
    cube = vtk.vtkCubeSource()
    cube.SetXLength(40)
    cube.SetYLength(30)
    cube.SetZLength(20)
    cubeMapper = vtk.vtkPolyDataMapper()
    cubeMapper.SetInputConnection(cube.GetOutputPort())

    # create a plane to cut,here it cuts in the XZ direction (xz normal=(1,0,0);XY =(0,0,1),YZ =(0,1,0)
    plane = vtk.vtkPlane()
    plane.SetOrigin(10, 0, 0)
    plane.SetNormal(1, 0, 0)

    # create cutter
    cutter = vtk.vtkCutter()
    cutter.SetCutFunction(plane)
    cutter.SetInputConnection(cube.GetOutputPort())
    cutter.Update()
    cutterMapper = vtk.vtkPolyDataMapper()
    cutterMapper.SetInputConnection(cutter.GetOutputPort())

    # create plane actor
    planeActor = vtk.vtkActor()
    planeActor.GetProperty().SetColor(colors.GetColor3d('Yellow'))
    planeActor.GetProperty().SetLineWidth(2)
    planeActor.GetProperty().SetAmbient(1.0)
    planeActor.GetProperty().SetDiffuse(0.0)
    planeActor.SetMapper(cutterMapper)

    # create cube actor
    cubeActor = vtk.vtkActor()
    cubeActor.GetProperty().SetColor(colors.GetColor3d('Aquamarine'))
    cubeActor.GetProperty().SetOpacity(0.5)
    cubeActor.SetMapper(cubeMapper)

    # create renderers and add actors of plane and cube
    ren = vtk.vtkRenderer()
    ren.AddActor(planeActor)
    ren.AddActor(cubeActor)
    ren.SetBackground(colors.GetColor3d('Silver'))

    # Add renderer to renderwindow and render
    renWin = vtk.vtkRenderWindow()
    renWin.AddRenderer(ren)
    renWin.SetSize(600, 600)
    renWin.SetWindowName('Cutter')
    renWin.Render()

    iren = vtk.vtkRenderWindowInteractor()
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
