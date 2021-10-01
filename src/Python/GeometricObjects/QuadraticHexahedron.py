#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkMinimalStandardRandomSequence,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import (
    vtkQuadraticHexahedron,
    vtkUnstructuredGrid
)
from vtkmodules.vtkFiltersCore import vtkGlyph3D
from vtkmodules.vtkFiltersGeneral import vtkTessellatorFilter
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    namedColors = vtkNamedColors()

    uGrid = MakeQuadraticHexahedron()

    tessellate = vtkTessellatorFilter()
    tessellate.SetInputData(uGrid)
    tessellate.SetMaximumNumberOfSubdivisions(2)

    mapper = vtkDataSetMapper()
    mapper.SetInputConnection(tessellate.GetOutputPort())
    mapper.ScalarVisibilityOff()

    # Create an actor for the grid
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetDiffuseColor(
        namedColors.GetColor3d('Tomato'))
    actor.GetProperty().SetEdgeColor(
        namedColors.GetColor3d('IvoryBlack'))
    actor.GetProperty().EdgeVisibilityOn()

    sphereSource = vtkSphereSource()
    sphereSource.SetRadius(0.02)

    glyph3D = vtkGlyph3D()
    glyph3D.SetInputData(uGrid)
    glyph3D.SetSourceConnection(sphereSource.GetOutputPort())
    glyph3D.ScalingOff()
    glyph3D.Update()

    glyph3DMapper = vtkDataSetMapper()
    glyph3DMapper.SetInputConnection(glyph3D.GetOutputPort())
    glyph3DMapper.ScalarVisibilityOff()

    glyph3DActor = vtkActor()
    glyph3DActor.SetMapper(glyph3DMapper)
    glyph3DActor.GetProperty().SetColor(
        namedColors.GetColor3d('Banana'))

    # Visualize
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName('QuadraticHexahedron')
    renderWindow.AddRenderer(renderer)
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.AddActor(glyph3DActor)
    renderer.SetBackground(namedColors.GetColor3d('SlateGray'))

    renderWindow.Render()
    interactor.Start()


def MakeQuadraticHexahedron():
    aHexahedron = vtkQuadraticHexahedron()
    points = vtkPoints()

    pcoords = aHexahedron.GetParametricCoords()
    rng = vtkMinimalStandardRandomSequence()
    points.SetNumberOfPoints(aHexahedron.GetNumberOfPoints())
    rng.SetSeed(5070)  # for testing
    for i in range(0, aHexahedron.GetNumberOfPoints()):
        perturbation = [0.0] * 3
        for j in range(0, 3):
            rng.Next()
            perturbation[j] = rng.GetRangeValue(-0.1, 0.1)
        aHexahedron.GetPointIds().SetId(i, i)
        points.SetPoint(i, pcoords[3 * i] + perturbation[0],
                        pcoords[3 * i + 1] + perturbation[1],
                        pcoords[3 * i + 2] + perturbation[2])

    # Add the points and hexahedron to an unstructured grid
    uGrid = vtkUnstructuredGrid()
    uGrid.SetPoints(points)
    uGrid.InsertNextCell(aHexahedron.GetCellType(), aHexahedron.GetPointIds())

    return uGrid


if __name__ == '__main__':
    main()
