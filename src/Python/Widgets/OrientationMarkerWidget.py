#!/usr/bin/env python
# Contributed by Eric E Monson

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkCubeSource
from vtkmodules.vtkInteractionWidgets import vtkOrientationMarkerWidget
from vtkmodules.vtkRenderingAnnotation import vtkAnnotatedCubeActor
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # colors.SetColor('bkg', [0.2, 0.3, 0.7, 1.0])

    # create a rendering window and renderer
    ren = vtkRenderer()
    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(ren)
    ren_win.SetWindowName('OrientationMarkerWidget')

    # create a renderwindowinteractor
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    cube = vtkCubeSource()
    cube.SetXLength(200)
    cube.SetYLength(200)
    cube.SetZLength(200)
    cube.Update()
    cm = vtkPolyDataMapper()
    cm.SetInputConnection(cube.GetOutputPort())
    ca = vtkActor()
    ca.SetMapper(cm)
    ca.GetProperty().SetColor(colors.GetColor3d("BurlyWood"))
    ca.GetProperty().EdgeVisibilityOn()
    ca.GetProperty().SetEdgeColor(colors.GetColor3d("Red"))

    # assign actor to the renderer
    ren.AddActor(ca)
    ren.SetBackground(colors.GetColor3d('CornflowerBlue'))

    axes_actor = vtkAnnotatedCubeActor()
    axes_actor.SetXPlusFaceText('L')
    axes_actor.SetXMinusFaceText('R')
    axes_actor.SetYMinusFaceText('I')
    axes_actor.SetYPlusFaceText('S')
    axes_actor.SetZMinusFaceText('P')
    axes_actor.SetZPlusFaceText('A')
    axes_actor.GetTextEdgesProperty().SetColor(colors.GetColor3d("Yellow"))
    axes_actor.GetTextEdgesProperty().SetLineWidth(2)
    axes_actor.GetCubeProperty().SetColor(colors.GetColor3d("Blue"))
    axes = vtkOrientationMarkerWidget()
    axes.SetOrientationMarker(axes_actor)
    axes.SetInteractor(iren)
    axes.EnabledOn()
    axes.InteractiveOn()
    ren.ResetCamera()

    # enable user interface interactor
    iren.Initialize()
    ren_win.Render()
    ren.GetActiveCamera().Azimuth(45)
    ren.GetActiveCamera().Elevation(30)
    ren_win.Render()
    iren.Start()


if __name__ == '__main__':
    main()
