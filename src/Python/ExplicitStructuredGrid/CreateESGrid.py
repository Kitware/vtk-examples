#!/usr/bin/env python

import numpy as np
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkExplicitStructuredGrid
)
from vtkmodules.vtkFiltersCore import (
    vtkExplicitStructuredGridToUnstructuredGrid,
    vtkUnstructuredGridToExplicitStructuredGrid
)
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleRubberBandPick
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def create_explicit_structured_grid(dimensions, spacing=(1, 1, 1)):
    """Create an explicit structured grid.

    Parameters
    ----------
    dimensions : tuple(int, int, int)
        The number of points in the I, J and K directions.
    spacing : tuple(int, int, int)
        The spacing between points in the I, J and K directions.

    Returns
    -------
    grid : vtkExplicitStructuredGrid
        An explicit structured grid.

    """
    ni, nj, nk = dimensions
    si, sj, sk = spacing

    points = vtkPoints()
    for z in range(0, nk * sk, sk):
        for y in range(0, nj * sj, sj):
            for x in range(0, ni * si, si):
                points.InsertNextPoint((x, y, z))

    cells = vtkCellArray()
    for k in range(0, nk - 1):
        for j in range(0, nj - 1):
            for i in range(0, ni - 1):
                multi_index = ([i, i + 1, i + 1, i, i, i + 1, i + 1, i],
                               [j, j, j + 1, j + 1, j, j, j + 1, j + 1],
                               [k, k, k, k, k + 1, k + 1, k + 1, k + 1])
                pts = np.ravel_multi_index(multi_index, dimensions, order='F')
                cells.InsertNextCell(8, pts)

    grid = vtkExplicitStructuredGrid()
    grid.SetDimensions(ni, nj, nk)
    grid.SetPoints(points)
    grid.SetCells(cells)
    return grid


def convert_to_unstructured_grid(grid):
    """Convert explicit structured grid to unstructured grid.

    Parameters
    ----------
    grid : vtkExplicitStructuredGrid
        An explicit structured grid.

    Returns
    -------
    vtkUnstructuredGrid
        An unstructured grid.

    """
    converter = vtkExplicitStructuredGridToUnstructuredGrid()
    converter.SetInputData(grid)
    converter.Update()
    return converter.GetOutput()


def convert_to_explicit_structured_grid(grid):
    """Convert unstructured grid to explicit structured grid.

    Parameters
    ----------
    grid : UnstructuredGrid
        An unstructured grid.

    Returns
    -------
    vtkExplicitStructuredGrid
        An explicit structured grid. The ``'BLOCK_I'``, ``'BLOCK_J'`` and
        ``'BLOCK_K'`` cell arrays are required.

    """
    converter = vtkUnstructuredGridToExplicitStructuredGrid()
    converter.SetInputData(grid)
    converter.SetInputArrayToProcess(0, 0, 0, 1, 'BLOCK_I')
    converter.SetInputArrayToProcess(1, 0, 0, 1, 'BLOCK_J')
    converter.SetInputArrayToProcess(2, 0, 0, 1, 'BLOCK_K')
    converter.Update()
    return converter.GetOutput()


def main():
    grid = create_explicit_structured_grid((5, 6, 7), (20, 10, 1))
    grid = convert_to_unstructured_grid(grid)
    grid = convert_to_explicit_structured_grid(grid)

    mapper = vtkDataSetMapper()
    mapper.SetInputData(grid)

    colors = vtkNamedColors()

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().EdgeVisibilityOn()
    actor.GetProperty().LightingOff()
    actor.GetProperty().SetColor(colors.GetColor3d('Seashell'))

    renderer = vtkRenderer()
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('DarkSlateGray'))

    window = vtkRenderWindow()
    window.AddRenderer(renderer)
    window.SetWindowName('CreateESGrid')
    window.SetSize(1024, 768)
    window.Render()

    camera = renderer.GetActiveCamera()
    camera.SetPosition(8.383354, -72.468670, 94.262605)
    camera.SetFocalPoint(42.295234, 21.111537, -0.863606)
    camera.SetViewUp(0.152863, 0.676710, 0.720206)
    camera.SetDistance(137.681759)
    camera.SetClippingRange(78.173985, 211.583658)

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(window)
    interactor.SetInteractorStyle(vtkInteractorStyleRubberBandPick())
    window.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
