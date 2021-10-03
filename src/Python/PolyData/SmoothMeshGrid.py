#!/usr/bin/env python

import numpy
# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkMinimalStandardRandomSequence,
    vtkPoints,
    vtkUnsignedCharArray
)
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData,
    vtkTriangle
)
from vtkmodules.vtkFiltersCore import vtkCleanPolyData
from vtkmodules.vtkFiltersModeling import (
    vtkButterflySubdivisionFilter,
    vtkLoopSubdivisionFilter
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    nc = vtkNamedColors()

    # Make a 32 x 32 grid
    size = 32

    rn = vtkMinimalStandardRandomSequence()
    rn.SetSeed(1)

    # Define z values for the topography (random height)
    topography = numpy.zeros([size, size])
    for i in range(size):
        for j in range(size):
            topography[i][j] = rn.GetRangeValue(0, 5)
            rn.Next()

    # Define points, triangles and colors
    colors = vtkUnsignedCharArray()
    colors.SetNumberOfComponents(3)
    points = vtkPoints()
    triangles = vtkCellArray()

    # Build the meshgrid manually
    count = 0
    for i in range(size - 1):
        for j in range(size - 1):
            z1 = topography[i][j]
            z2 = topography[i][j + 1]
            z3 = topography[i + 1][j]

            # Triangle 1
            points.InsertNextPoint(i, j, z1)
            points.InsertNextPoint(i, (j + 1), z2)
            points.InsertNextPoint((i + 1), j, z3)

            triangle = vtkTriangle()
            triangle.GetPointIds().SetId(0, count)
            triangle.GetPointIds().SetId(1, count + 1)
            triangle.GetPointIds().SetId(2, count + 2)

            triangles.InsertNextCell(triangle)

            z1 = topography[i][j + 1]
            z2 = topography[i + 1][j + 1]
            z3 = topography[i + 1][j]

            # Triangle 2
            points.InsertNextPoint(i, (j + 1), z1)
            points.InsertNextPoint((i + 1), (j + 1), z2)
            points.InsertNextPoint((i + 1), j, z3)

            triangle = vtkTriangle()
            triangle.GetPointIds().SetId(0, count + 3)
            triangle.GetPointIds().SetId(1, count + 4)
            triangle.GetPointIds().SetId(2, count + 5)

            count += 6

            triangles.InsertNextCell(triangle)

            # Add some color
            r = [int(i / float(size) * 255), int(j / float(size) * 255), 0]
            colors.InsertNextTypedTuple(r)
            colors.InsertNextTypedTuple(r)
            colors.InsertNextTypedTuple(r)
            colors.InsertNextTypedTuple(r)
            colors.InsertNextTypedTuple(r)
            colors.InsertNextTypedTuple(r)

    # Create a polydata object
    trianglePolyData = vtkPolyData()

    # Add the geometry and topology to the polydata
    trianglePolyData.SetPoints(points)
    trianglePolyData.GetPointData().SetScalars(colors)
    trianglePolyData.SetPolys(triangles)

    # Clean the polydata so that the edges are shared !
    cleanPolyData = vtkCleanPolyData()
    cleanPolyData.SetInputData(trianglePolyData)

    # Use a filter to smooth the data (will add triangles and smooth)
    # Use two different filters to show the difference
    smooth_loop = vtkLoopSubdivisionFilter()
    smooth_loop.SetNumberOfSubdivisions(3)
    smooth_loop.SetInputConnection(cleanPolyData.GetOutputPort())
    smooth_butterfly = vtkButterflySubdivisionFilter()
    smooth_butterfly.SetNumberOfSubdivisions(3)
    smooth_butterfly.SetInputConnection(cleanPolyData.GetOutputPort())

    # Create a mapper and actor for initial dataset
    mapper = vtkPolyDataMapper()
    mapper.SetInputData(trianglePolyData)
    actor = vtkActor()
    actor.SetMapper(mapper)

    # Create a mapper and actor for smoothed dataset (vtkLoopSubdivisionFilter)
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(smooth_loop.GetOutputPort())
    actor_loop = vtkActor()
    actor_loop.SetMapper(mapper)
    actor_loop.SetPosition(32, 0, 0)

    # Create a mapper and actor for smoothed dataset (vtkButterflySubdivisionFilter)
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(smooth_butterfly.GetOutputPort())
    actor_butterfly = vtkActor()
    actor_butterfly.SetMapper(mapper)
    actor_butterfly.SetPosition(64, 0, 0)

    # Visualise
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add actors and render
    renderer.AddActor(actor)
    renderer.AddActor(actor_loop)
    renderer.AddActor(actor_butterfly)
    renderer.SetBackground(nc.GetColor3d('AliceBlue'))

    renderWindow.SetSize(900, 300)
    renderWindow.Render()
    renderer.GetActiveCamera().Elevation(-45)
    renderer.GetActiveCamera().Zoom(3)
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
