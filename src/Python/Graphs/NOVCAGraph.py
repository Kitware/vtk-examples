#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonCore import (
    vtkIntArray,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import (
    VTK_LINE,
    vtkCellArray,
    vtkUnstructuredGrid
)
from vtkmodules.vtkFiltersSources import vtkPointSource
from vtkmodules.vtkIOXML import vtkXMLUnstructuredGridWriter


def get_program_parameters():
    import argparse
    description = 'Building a graph using Unstructured Grid & dumping it into a vtk file.'
    epilogue = '''
        Building a graph using Unstructured Grid & dumping it into a vtk file, to be visualized using ParaView.
        
        The generated file can then be used in ParaView/VisIt.
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename', help='A required vtk filename, e.g. vertex.vtu.', nargs='?', const='vertex.vtu',
                        type=str, default='testVertex.vtu')
    args = parser.parse_args()
    return args.filename


def main():
    fn = get_program_parameters()

    pointSource = vtkPointSource()
    pointSource.Update()

    # Create an integer array to store vertex id data & link it with its degree value as a scalar.
    degree = vtkIntArray()
    degree.SetNumberOfComponents(1)
    degree.SetName('degree')
    degree.SetNumberOfTuples(7)
    degree.SetValue(0, 2)
    degree.SetValue(1, 1)
    degree.SetValue(2, 3)
    degree.SetValue(3, 3)
    degree.SetValue(4, 4)
    degree.SetValue(5, 2)
    degree.SetValue(6, 1)

    pointSource.GetOutput().GetPointData().AddArray(degree)

    # Assign co-ordinates for vertices
    Points = vtkPoints()
    Points.InsertNextPoint(0, 1, 0)
    Points.InsertNextPoint(0, 0, 0)
    Points.InsertNextPoint(1, 1, 0)
    Points.InsertNextPoint(1, 0, 0)
    Points.InsertNextPoint(2, 1, 0)
    Points.InsertNextPoint(2, 0, 0)
    Points.InsertNextPoint(3, 0, 0)

    # Establish the specified edges using CellArray
    line = vtkCellArray()
    line.Allocate(8)
    line.InsertNextCell(2)
    line.InsertCellPoint(0)
    line.InsertCellPoint(1)
    line.InsertNextCell(2)
    line.InsertCellPoint(0)
    line.InsertCellPoint(2)
    line.InsertNextCell(2)
    line.InsertCellPoint(2)
    line.InsertCellPoint(3)
    line.InsertNextCell(2)
    line.InsertCellPoint(2)
    line.InsertCellPoint(4)
    line.InsertNextCell(2)
    line.InsertCellPoint(3)
    line.InsertCellPoint(4)
    line.InsertNextCell(2)
    line.InsertCellPoint(3)
    line.InsertCellPoint(5)
    line.InsertNextCell(2)
    line.InsertCellPoint(4)
    line.InsertCellPoint(5)
    line.InsertNextCell(2)
    line.InsertCellPoint(4)
    line.InsertCellPoint(6)

    # Add the vertices and edges to unstructured Grid
    G = vtkUnstructuredGrid()
    G.GetPointData().SetScalars(degree)
    G.SetPoints(Points)
    G.SetCells(VTK_LINE, line)

    # Dump the graph in VTK unstructured format (.vtu)
    gw = vtkXMLUnstructuredGridWriter()
    gw.SetFileName(fn)
    gw.SetInputData(G)
    gw.Write()
    print('---> ')

    print('Feed the vertex.vtu file in ParaView/VisIt.')


if __name__ == '__main__':
    main()
