#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingFreeType
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import (
    vtkArrowSource,
    vtkConeSource,
    vtkCubeSource,
    vtkCylinderSource,
    vtkDiskSource,
    vtkLineSource,
    vtkRegularPolygonSource,
    vtkSphereSource
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkActor2D,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTextMapper,
    vtkTextProperty
)


def main():
    colors = vtkNamedColors()

    # Set the background color.
    colors.SetColor("BkgColor", [51, 77, 102, 255])

    # Create container to hold the 3D object generators (sources)
    geometricObjectSources = list()

    # Populate the container with the various object sources to be demonstrated
    geometricObjectSources.append(vtkArrowSource())
    geometricObjectSources.append(vtkConeSource())
    geometricObjectSources.append(vtkCubeSource())
    geometricObjectSources.append(vtkCylinderSource())
    geometricObjectSources.append(vtkDiskSource())
    geometricObjectSources.append(vtkLineSource())
    geometricObjectSources.append(vtkRegularPolygonSource())
    geometricObjectSources.append(vtkSphereSource())

    # Create containers for the remaining nodes of each pipeline
    mappers = list()
    actors = list()
    textmappers = list()
    textactors = list()

    # Create a common text property.
    textProperty = vtkTextProperty()
    textProperty.SetFontSize(16)
    textProperty.SetJustificationToCentered()
    textProperty.SetColor(colors.GetColor3d('LightGoldenrodYellow'))

    # Create a mapper and actor for each object and the corresponding text label
    for i in range(0, len(geometricObjectSources)):
        geometricObjectSources[i].Update()

        mappers.append(vtkPolyDataMapper())
        mappers[i].SetInputConnection(geometricObjectSources[i].GetOutputPort())

        actors.append(vtkActor())
        actors[i].SetMapper(mappers[i])
        actors[i].GetProperty().SetColor(colors.GetColor3d('PeachPuff'))

        textmappers.append(vtkTextMapper())
        textmappers[i].SetInput(
            geometricObjectSources[i].GetClassName())  # set text label to the name of the object source
        textmappers[i].SetTextProperty(textProperty)

        textactors.append(vtkActor2D())
        textactors[i].SetMapper(textmappers[i])
        textactors[i].SetPosition(120, 16)  # Note: the position of an Actor2D is specified in display coordinates

    # Define size of the grid that will hold the objects
    gridCols = 3
    gridRows = 3
    # Define side length (in pixels) of each renderer square
    rendererSize = 300

    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName('GeometricObjectsDemo')
    renderWindow.SetSize(rendererSize * gridCols, rendererSize * gridRows)

    # Set up a grid of viewports for each renderer
    for row in range(0, gridRows):
        for col in range(0, gridCols):
            index = row * gridCols + col

            # Create a renderer for this grid cell
            renderer = vtkRenderer()
            renderer.SetBackground(colors.GetColor3d('BkgColor'))

            # Set the renderer's viewport dimensions (xmin, ymin, xmax, ymax) within the render window.
            # Note that for the Y values, we need to subtract the row index from gridRows
            # because the viewport Y axis points upwards, but we want to draw the grid from top to down
            viewport = [
                float(col) / gridCols,
                float(gridRows - row - 1) / gridRows,
                float(col + 1) / gridCols,
                float(gridRows - row) / gridRows
            ]
            renderer.SetViewport(viewport)

            # Add the corresponding actor and label for this grid cell, if they exist
            if index < len(geometricObjectSources):
                renderer.AddActor(actors[index])
                renderer.AddActor(textactors[index])
                renderer.ResetCameraClippingRange()

            renderWindow.AddRenderer(renderer)

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    renderWindow.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
