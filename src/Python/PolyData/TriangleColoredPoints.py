#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkPoints,
    vtkUnsignedCharArray
)
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData
)
from vtkmodules.vtkIOXML import vtkXMLPolyDataWriter


def get_program_parameters():
    import argparse
    description = 'Generate a triangle with colored points and write it to a .vtp file.'
    epilogue = '''
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename', help='A required vtp filename.', nargs='?',
                        const='TestTriangleColoredPoints.vtp',
                        type=str, default='TestTriangleColoredPoints.vtp')
    args = parser.parse_args()
    return args.filename


def main():
    colors = vtkNamedColors()

    filename = get_program_parameters()

    # setup points and vertices
    Points = vtkPoints()
    Vertices = vtkCellArray()

    id = Points.InsertNextPoint(1.0, 0.0, 0.0)
    Vertices.InsertNextCell(1)
    Vertices.InsertCellPoint(id)
    id = Points.InsertNextPoint(0.0, 0.0, 0.0)
    Vertices.InsertNextCell(1)
    Vertices.InsertCellPoint(id)
    id = Points.InsertNextPoint(0.0, 1.0, 0.0)
    Vertices.InsertNextCell(1)
    Vertices.InsertCellPoint(id)

    # setup colors
    Colors = vtkUnsignedCharArray()
    Colors.SetNumberOfComponents(3)
    Colors.SetName("Colors")
    Colors.InsertNextTuple3(*colors.GetColor3ub('Red'))
    Colors.InsertNextTuple3(*colors.GetColor3ub('LimeGreen'))
    Colors.InsertNextTuple3(*colors.GetColor3ub('Blue'))

    polydata = vtkPolyData()
    polydata.SetPoints(Points)
    polydata.SetVerts(Vertices)
    polydata.GetPointData().SetScalars(Colors)
    polydata.Modified()

    writer = vtkXMLPolyDataWriter()
    writer.SetFileName(filename)
    writer.SetInputData(polydata)
    writer.Write()


if __name__ == '__main__':
    main()
