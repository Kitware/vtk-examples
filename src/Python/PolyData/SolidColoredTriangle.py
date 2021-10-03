#!/usr/bin/env python

from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkPoints,
    vtkUnsignedCharArray
)
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData,
    vtkTriangle
)
from vtkmodules.vtkIOXML import vtkXMLPolyDataWriter


def get_program_parameters():
    import argparse
    description = 'Generate a solid colored triangle and write it to a .vtp file.'
    epilogue = '''
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename', help='A required vtp filename.', nargs='?',
                        const='TestSolidColorTriangle.vtp',
                        type=str, default='TestSolidColorTriangle.vtp')
    args = parser.parse_args()
    return args.filename


def main():
    colors = vtkNamedColors()

    filename = get_program_parameters()

    # setup points and vertices
    Points = vtkPoints()
    Triangles = vtkCellArray()

    Points.InsertNextPoint(1.0, 0.0, 0.0)
    Points.InsertNextPoint(0.0, 0.0, 0.0)
    Points.InsertNextPoint(0.0, 1.0, 0.0)

    Triangle = vtkTriangle()
    Triangle.GetPointIds().SetId(0, 0)
    Triangle.GetPointIds().SetId(1, 1)
    Triangle.GetPointIds().SetId(2, 2)
    Triangles.InsertNextCell(Triangle)

    # Setup colors (setting the name to "Colors" is nice but not necessary)
    Colors = vtkUnsignedCharArray()
    Colors.SetNumberOfComponents(3)
    Colors.SetName("Colors")
    Colors.InsertNextTuple3(*colors.GetColor3ub('Red'))

    polydata = vtkPolyData()
    polydata.SetPoints(Points)
    polydata.SetPolys(Triangles)

    polydata.GetCellData().SetScalars(Colors)
    polydata.Modified()

    writer = vtkXMLPolyDataWriter()
    writer.SetFileName(filename)
    writer.SetInputData(polydata)
    writer.Write()


if __name__ == '__main__':
    main()
