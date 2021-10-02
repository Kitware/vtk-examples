# !/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonDataModel import (
    vtkConvexPointSet,
    vtkPolyData,
    vtkUnstructuredGrid
)
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkGlyph3DMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    cps = vtkConvexPointSet()
    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(1, 0, 0)
    points.InsertNextPoint(1, 1, 0)
    points.InsertNextPoint(0, 1, 0)
    points.InsertNextPoint(0, 0, 1)
    points.InsertNextPoint(1, 0, 1)
    points.InsertNextPoint(1, 1, 1)
    points.InsertNextPoint(0, 1, 1)
    points.InsertNextPoint(0.5, 0, 0)
    points.InsertNextPoint(1, 0.5, 0)
    points.InsertNextPoint(0.5, 1, 0)
    points.InsertNextPoint(0, 0.5, 0)
    points.InsertNextPoint(0.5, 0.5, 0)

    for i in range(0, 13):
        cps.GetPointIds().InsertId(i, i)

    ug = vtkUnstructuredGrid()
    ug.Allocate(1, 1)
    ug.InsertNextCell(cps.GetCellType(), cps.GetPointIds())
    ug.SetPoints(points)

    colors = vtkNamedColors()

    mapper = vtkDataSetMapper()
    mapper.SetInputData(ug)

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d("Tomato"))
    actor.GetProperty().SetLineWidth(3)
    actor.GetProperty().EdgeVisibilityOn()

    # Glyph the points
    sphere = vtkSphereSource()
    sphere.SetPhiResolution(21)
    sphere.SetThetaResolution(21)
    sphere.SetRadius(.03)

    # Create a polydata to store everything in
    polyData = vtkPolyData()
    polyData.SetPoints(points)

    pointMapper = vtkGlyph3DMapper()
    pointMapper.SetInputData(polyData)
    pointMapper.SetSourceConnection(sphere.GetOutputPort())

    pointActor = vtkActor()
    pointActor.SetMapper(pointMapper)
    pointActor.GetProperty().SetColor(colors.GetColor3d("Peacock"))

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName("ConvexPointSet")
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actors to the scene
    renderer.AddActor(actor)
    renderer.AddActor(pointActor)
    renderer.SetBackground(colors.GetColor3d("Silver"))

    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(210)
    renderer.GetActiveCamera().Elevation(30)
    renderer.ResetCameraClippingRange()

    # Render and interact
    renderWindow.SetSize(640, 480)
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
