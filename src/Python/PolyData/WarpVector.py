#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkDoubleArray,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkLine,
    vtkPolyData
)
from vtkmodules.vtkFiltersGeneral import vtkWarpVector
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    points = vtkPoints()
    points.InsertNextPoint(0.0, 0.0, 0.0)
    points.InsertNextPoint(1.0, 0.0, 0.0)
    points.InsertNextPoint(2.0, 0.0, 0.0)
    points.InsertNextPoint(3.0, 0.0, 0.0)
    points.InsertNextPoint(4.0, 0.0, 0.0)

    lines = vtkCellArray()
    line = vtkLine()
    line.GetPointIds().SetId(0, 0)
    line.GetPointIds().SetId(1, 1)
    lines.InsertNextCell(line)
    line.GetPointIds().SetId(0, 1)
    line.GetPointIds().SetId(1, 2)
    lines.InsertNextCell(line)
    line.GetPointIds().SetId(0, 2)
    line.GetPointIds().SetId(1, 3)
    lines.InsertNextCell(line)
    line.GetPointIds().SetId(0, 3)
    line.GetPointIds().SetId(1, 4)
    lines.InsertNextCell(line)

    warpData = vtkDoubleArray()
    warpData.SetNumberOfComponents(3)
    warpData.SetName("warpData")
    warp = [0.0, 0.0, 0.0]
    warp[1] = 0.0
    warpData.InsertNextTuple(warp)
    warp[1] = 0.1
    warpData.InsertNextTuple(warp)
    warp[1] = 0.3
    warpData.InsertNextTuple(warp)
    warp[1] = 0.0
    warpData.InsertNextTuple(warp)
    warp[1] = 0.1
    warpData.InsertNextTuple(warp)

    polydata = vtkPolyData()
    polydata.SetPoints(points)
    polydata.SetLines(lines)
    polydata.GetPointData().AddArray(warpData)
    polydata.GetPointData().SetActiveVectors(warpData.GetName())

    # WarpVector will use the array marked as active vector in polydata
    # it has to be a 3 component array
    # with the same number of tuples as points in polydata
    warpVector = vtkWarpVector()
    warpVector.SetInputData(polydata)
    warpVector.Update()

    mapper = vtkPolyDataMapper()
    mapper.SetInputData(warpVector.GetPolyDataOutput())

    actor = vtkActor()
    actor.SetMapper(mapper)

    renderer = vtkRenderer()
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('cobalt_green'))

    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('WarpVector')

    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
