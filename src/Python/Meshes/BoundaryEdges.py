#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkFeatureEdges
from vtkmodules.vtkFiltersSources import vtkDiskSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()
    diskSource = vtkDiskSource()
    diskSource.Update()

    featureEdges = vtkFeatureEdges()
    featureEdges.SetInputConnection(diskSource.GetOutputPort())
    featureEdges.BoundaryEdgesOn()
    featureEdges.FeatureEdgesOff()
    featureEdges.ManifoldEdgesOff()
    featureEdges.NonManifoldEdgesOff()
    featureEdges.ColoringOn()
    featureEdges.Update()

    # Visualize
    edgeMapper = vtkPolyDataMapper()
    edgeMapper.SetInputConnection(featureEdges.GetOutputPort())
    edgeActor = vtkActor()
    edgeActor.SetMapper(edgeMapper)

    diskMapper = vtkPolyDataMapper()
    diskMapper.SetInputConnection(diskSource.GetOutputPort())
    diskActor = vtkActor()
    diskActor.SetMapper(diskMapper)
    diskActor.GetProperty().SetColor(colors.GetColor3d('Gray'))

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName('BoundaryEdges')

    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(edgeActor)
    renderer.AddActor(diskActor)
    renderer.SetBackground(colors.GetColor3d('DimGray'))
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
