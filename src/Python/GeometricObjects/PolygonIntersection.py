#!/usr/bin/env python
from __future__ import print_function

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonCore import (
    mutable,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import vtkPolygon


def main():
    # Create a square in the x-y plane.
    points = vtkPoints()
    points.InsertNextPoint(0.0, 0.0, 0.0)
    points.InsertNextPoint(1.0, 0.0, 0.0)
    points.InsertNextPoint(1.0, 1.0, 0.0)
    points.InsertNextPoint(0.0, 1.0, 0.0)

    # Create the polygon
    polygon = vtkPolygon()
    polygon.GetPoints().DeepCopy(points)
    polygon.GetPointIds().SetNumberOfIds(4)  # The 4 corners of the square
    for i in range(4):
        polygon.GetPointIds().SetId(i, i)

    # Inputs
    p1 = [0.1, 0, -1.0]
    p2 = [0.1, 0, 1.0]
    tolerance = 0.001

    # Outputs
    t = mutable(0)  # Parametric coordinate of intersection (0 (corresponding to p1) to 1 (corresponding to p2))
    x = [0.0, 0.0, 0.0]
    pcoords = [0.0, 0.0, 0.0]
    subId = mutable(0)
    iD = polygon.IntersectWithLine(p1, p2, tolerance, t, x, pcoords, subId)

    print('intersected? ', 'Yes' if iD == 1 else 'No')
    print('intersection: ', x)


if __name__ == '__main__':
    main()
