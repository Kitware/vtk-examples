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
    vtkQuadraticTetra,
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

    uGrid = MakeQuadraticTetra()

    tessellate = vtkTessellatorFilter()
    tessellate.SetInputData(uGrid)

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
    renderWindow.SetWindowName('QuadraticTetra')
    renderWindow.AddRenderer(renderer)
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.AddActor(glyph3DActor)
    renderer.SetBackground(namedColors.GetColor3d('SlateGray'))

    renderWindow.Render()
    interactor.Start()


def MakeQuadraticTetra():
    aTetra = vtkQuadraticTetra()
    points = vtkPoints()

    pcoords = aTetra.GetParametricCoords()
    rng = vtkMinimalStandardRandomSequence()
    points.SetNumberOfPoints(aTetra.GetNumberOfPoints())
    rng.SetSeed(5070)  # for testing
    for i in range(0, aTetra.GetNumberOfPoints()):
        perturbation = [0.0] * 3
        for j in range(0, 3):
            rng.Next()
            perturbation[j] = rng.GetRangeValue(-0.1, 0.1)
        aTetra.GetPointIds().SetId(i, i)
        points.SetPoint(i, pcoords[3 * i] + perturbation[0],
                        pcoords[3 * i + 1] + perturbation[1],
                        pcoords[3 * i + 2] + perturbation[2])

    # Add the points and tetra to an unstructured grid
    uGrid = vtkUnstructuredGrid()
    uGrid.SetPoints(points)
    uGrid.InsertNextCell(aTetra.GetCellType(), aTetra.GetPointIds())

    return uGrid


if __name__ == '__main__':
    main()
