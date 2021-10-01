#!/usr/bin/env python

from vtkmodules.vtkCommonDataModel import vtkPolyData
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkIOXML import vtkXMLPolyDataWriter


def main():
    # setup sphere
    sphereSource = vtkSphereSource()
    sphereSource.Update()

    polydata = vtkPolyData()
    polydata.ShallowCopy(sphereSource.GetOutput())

    normals = polydata.GetPointData().GetNormals()
    normals.SetName('TestN')

    writer = vtkXMLPolyDataWriter()
    writer.SetFileName('Test.vtp')
    writer.SetInputData(polydata)
    writer.Write()


if __name__ == '__main__':
    main()
