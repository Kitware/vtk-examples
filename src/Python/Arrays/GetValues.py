#!/usr/bin/env python

from vtkmodules.vtkCommonDataModel import vtkPolyData
from vtkmodules.vtkFiltersSources import vtkSphereSource


def main():
    # setup sphere
    sphereSource = vtkSphereSource()
    sphereSource.Update()

    polydata = vtkPolyData()
    polydata.ShallowCopy(sphereSource.GetOutput())

    normals = polydata.GetPointData().GetNormals()
    normal0 = normals.GetTuple3(0)

    print('Normal0: {:3.1f} {:3.1f} {:3.1f}'.format(normal0[0], normal0[1], normal0[2]))


if __name__ == '__main__':
    main()
