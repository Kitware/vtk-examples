#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonCore import (
    vtkIdList,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkLine,
    vtkPolyData
)


def main():
    origin = [0.0, 0.0, 0.0]
    p0 = [1.0, 0.0, 0.0]
    p1 = [0.0, 1.0, 0.0]
    p2 = [0.0, 1.0, 2.0]
    p3 = [1.0, 2.0, 3.0]

    # Create a vtkPoints object and store the points in it.
    points = vtkPoints()
    points.InsertNextPoint(origin)
    points.InsertNextPoint(p0)
    points.InsertNextPoint(p1)
    points.InsertNextPoint(p2)
    points.InsertNextPoint(p3)

    # Create a cell array to store the lines in and add the lines to it.
    lines = vtkCellArray()

    # Create four lines.
    for i in range(4):
        line = vtkLine()
        line.GetPointIds().SetId(0, i)
        line.GetPointIds().SetId(1, i + 1)
        lines.InsertNextCell(line)

    # Create a polydata to store everything in.
    linesPolyData = vtkPolyData()

    # Add the points to the dataset.
    linesPolyData.SetPoints(points)

    # Add the lines to the dataset.
    linesPolyData.SetLines(lines)

    print('There are {0} lines.'.format(linesPolyData.GetNumberOfLines()))

    linesPolyData.GetLines().InitTraversal()
    idList = vtkIdList()
    while (linesPolyData.GetLines().GetNextCell(idList)):
        print('Line has {0} points'.format(idList.GetNumberOfIds()))
        for pointId in range(idList.GetNumberOfIds() - 1):
            print('{0} {1}'.format(idList.GetId(pointId), idList.GetId(pointId + 1)))


if __name__ == '__main__':
    main()
