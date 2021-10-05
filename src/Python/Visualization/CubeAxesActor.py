#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkSuperquadricSource
from vtkmodules.vtkRenderingAnnotation import vtkCubeAxesActor
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    backgroundColor = colors.GetColor3d("DarkSlateGray")
    actorColor = colors.GetColor3d("Tomato")
    axis1Color = colors.GetColor3d("Salmon")
    axis2Color = colors.GetColor3d("PaleGreen")
    axis3Color = colors.GetColor3d("LightSkyBlue")

    # Create a superquadric
    superquadricSource = vtkSuperquadricSource()
    superquadricSource.SetPhiRoundness(3.1)
    superquadricSource.SetThetaRoundness(1.0)
    superquadricSource.Update()  # needed to GetBounds later

    renderer = vtkRenderer()

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(superquadricSource.GetOutputPort())

    superquadricActor = vtkActor()
    superquadricActor.SetMapper(mapper)
    superquadricActor.GetProperty().SetDiffuseColor(actorColor)
    superquadricActor.GetProperty().SetDiffuse(.7)
    superquadricActor.GetProperty().SetSpecular(.7)
    superquadricActor.GetProperty().SetSpecularPower(50.0)

    cubeAxesActor = vtkCubeAxesActor()
    cubeAxesActor.SetUseTextActor3D(1)
    cubeAxesActor.SetBounds(superquadricSource.GetOutput().GetBounds())
    cubeAxesActor.SetCamera(renderer.GetActiveCamera())
    cubeAxesActor.GetTitleTextProperty(0).SetColor(axis1Color)
    cubeAxesActor.GetTitleTextProperty(0).SetFontSize(48)
    cubeAxesActor.GetLabelTextProperty(0).SetColor(axis1Color)

    cubeAxesActor.GetTitleTextProperty(1).SetColor(axis2Color)
    cubeAxesActor.GetLabelTextProperty(1).SetColor(axis2Color)

    cubeAxesActor.GetTitleTextProperty(2).SetColor(axis3Color)
    cubeAxesActor.GetLabelTextProperty(2).SetColor(axis3Color)

    cubeAxesActor.DrawXGridlinesOn()
    cubeAxesActor.DrawYGridlinesOn()
    cubeAxesActor.DrawZGridlinesOn()
    cubeAxesActor.SetGridLineLocation(cubeAxesActor.VTK_GRID_LINES_FURTHEST)

    cubeAxesActor.XAxisMinorTickVisibilityOff()
    cubeAxesActor.YAxisMinorTickVisibilityOff()
    cubeAxesActor.ZAxisMinorTickVisibilityOff()

    cubeAxesActor.SetFlyModeToStaticEdges()
    renderer.AddActor(cubeAxesActor)
    renderer.AddActor(superquadricActor)
    renderer.GetActiveCamera().Azimuth(30)
    renderer.GetActiveCamera().Elevation(30)

    renderer.ResetCamera()
    renderer.SetBackground(backgroundColor)

    renderWindow = vtkRenderWindow()

    renderWindow.AddRenderer(renderer)
    renderWindow.SetSize(640, 480)
    renderWindow.SetWindowName('CubeAxesActor')

    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderWindow.Render()
    renderer.GetActiveCamera().Zoom(0.8)
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
