#!/usr/bin/env python

"""
This code is based on the VTK file: Examples/Modelling/Tcl/DelMesh.py.

This example demonstrates how to use 2D Delaunay triangulation.
We create a fancy image of a 2D Delaunay triangulation. Points are
 randomly generated.
"""

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkMinimalStandardRandomSequence,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import vtkPolyData
from vtkmodules.vtkFiltersCore import (
    vtkDelaunay2D,
    vtkGlyph3D,
    vtkTubeFilter
)
from vtkmodules.vtkFiltersExtraction import vtkExtractEdges
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Generate some "random" points.
    points = vtkPoints()
    randomSequence = vtkMinimalStandardRandomSequence()
    randomSequence.SetSeed(1)
    for i in range(0, 50):
        p1 = randomSequence.GetValue()
        randomSequence.Next()
        p2 = randomSequence.GetValue()
        randomSequence.Next()
        points.InsertPoint(i, p1, p2, 0.0)

    # Create a polydata with the points we just created.
    profile = vtkPolyData()
    profile.SetPoints(points)

    # Perform a 2D Delaunay triangulation on them.
    delny = vtkDelaunay2D()
    delny.SetInputData(profile)
    delny.SetTolerance(0.001)
    mapMesh = vtkPolyDataMapper()
    mapMesh.SetInputConnection(delny.GetOutputPort())
    meshActor = vtkActor()
    meshActor.SetMapper(mapMesh)
    meshActor.GetProperty().SetColor(colors.GetColor3d('MidnightBlue'))

    # We will now create a nice looking mesh by wrapping the edges in tubes,
    # and putting fat spheres at the points.
    extract = vtkExtractEdges()
    extract.SetInputConnection(delny.GetOutputPort())
    tubes = vtkTubeFilter()
    tubes.SetInputConnection(extract.GetOutputPort())
    tubes.SetRadius(0.01)
    tubes.SetNumberOfSides(6)
    mapEdges = vtkPolyDataMapper()
    mapEdges.SetInputConnection(tubes.GetOutputPort())
    edgeActor = vtkActor()
    edgeActor.SetMapper(mapEdges)
    edgeActor.GetProperty().SetColor(colors.GetColor3d('peacock'))
    edgeActor.GetProperty().SetSpecularColor(1, 1, 1)
    edgeActor.GetProperty().SetSpecular(0.3)
    edgeActor.GetProperty().SetSpecularPower(20)
    edgeActor.GetProperty().SetAmbient(0.2)
    edgeActor.GetProperty().SetDiffuse(0.8)

    ball = vtkSphereSource()
    ball.SetRadius(0.025)
    ball.SetThetaResolution(12)
    ball.SetPhiResolution(12)
    balls = vtkGlyph3D()
    balls.SetInputConnection(delny.GetOutputPort())
    balls.SetSourceConnection(ball.GetOutputPort())
    mapBalls = vtkPolyDataMapper()
    mapBalls.SetInputConnection(balls.GetOutputPort())
    ballActor = vtkActor()
    ballActor.SetMapper(mapBalls)
    ballActor.GetProperty().SetColor(colors.GetColor3d('hot_pink'))
    ballActor.GetProperty().SetSpecularColor(1, 1, 1)
    ballActor.GetProperty().SetSpecular(0.3)
    ballActor.GetProperty().SetSpecularPower(20)
    ballActor.GetProperty().SetAmbient(0.2)
    ballActor.GetProperty().SetDiffuse(0.8)

    # Create the rendering window, renderer, and interactive renderer.
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Add the actors to the renderer, set the background and size.
    ren.AddActor(ballActor)
    ren.AddActor(edgeActor)
    ren.SetBackground(colors.GetColor3d('AliceBlue'))
    renWin.SetSize(512, 512)
    renWin.SetWindowName('DelaunayMesh')

    ren.ResetCamera()
    ren.GetActiveCamera().Zoom(1.3)

    # Interact with the data.
    iren.Initialize()
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
