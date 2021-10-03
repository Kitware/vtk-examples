#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkLookupTable,
    vtkMinimalStandardRandomSequence,
    vtkPoints,
    vtkUnsignedCharArray
)
from vtkmodules.vtkCommonDataModel import vtkPolyData
from vtkmodules.vtkFiltersCore import vtkDelaunay2D
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    named_colors = vtkNamedColors()

    # Create a grid points
    points = vtkPoints()
    GridSize = 20;
    xx = 0.0
    yy = 0.0
    zz = 0.0
    rng = vtkMinimalStandardRandomSequence()
    rng.SetSeed(8775586)  # For testing.
    for x in range(0, GridSize):
        for y in range(0, GridSize):
            rng.Next()
            xx = x + rng.GetRangeValue(-0.2, 0.2)
            rng.Next()
            yy = y + rng.GetRangeValue(-0.2, 0.2)
            rng.Next()
            zz = rng.GetRangeValue(-0.5, 0.5)
            points.InsertNextPoint(xx, yy, zz)

    # Add the grid points to a polydata object
    inputPolyData = vtkPolyData()
    inputPolyData.SetPoints(points)

    # Triangulate the grid points
    delaunay = vtkDelaunay2D()
    delaunay.SetInputData(inputPolyData)
    delaunay.Update()
    outputPolyData = delaunay.GetOutput()

    bounds = 6 * [0.0]
    outputPolyData.GetBounds(bounds)

    # Find min and max z
    minz = bounds[4]
    maxz = bounds[5]

    print('minz: {:< 6.3}'.format(minz))
    print('maxz: {:< 6.3}'.format(maxz))

    # Create the color map
    colorLookupTable = vtkLookupTable()
    colorLookupTable.SetTableRange(minz, maxz)
    colorLookupTable.Build()

    # Generate the colors for each point based on the color map
    colors = vtkUnsignedCharArray()
    colors.SetNumberOfComponents(3)
    colors.SetName('Colors')

    print('There are ' + str(outputPolyData.GetNumberOfPoints()) + ' points.')

    for i in range(0, outputPolyData.GetNumberOfPoints()):
        p = 3 * [0.0]
        outputPolyData.GetPoint(i, p)

        dcolor = 3 * [0.0]
        colorLookupTable.GetColor(p[2], dcolor);
        # print( 'dcolor: {:<8.6} {:<8.6} {:<8.6}'.format(*dcolor))
        color = 3 * [0.0]
        for j in range(0, 3):
            color[j] = int(255.0 * dcolor[j])
        # print('color:  {:<8} {:<8} {:<8}'.format(*color))

        try:
            colors.InsertNextTupleValue(color)
        except AttributeError:
            # For compatibility with new VTK generic data arrays.
            colors.InsertNextTypedTuple(color)

    outputPolyData.GetPointData().SetScalars(colors)

    # Create a mapper and actor
    mapper = vtkPolyDataMapper()
    mapper.SetInputData(outputPolyData)

    actor = vtkActor()
    actor.SetMapper(mapper)

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('ColoredElevationMap')

    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actor to the scene
    renderer.AddActor(actor)
    renderer.SetBackground(named_colors.GetColor3d('DarkSlateGray'))

    # Render and interact
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
