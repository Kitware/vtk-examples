#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import (
    vtkCleanPolyData,
    vtkTriangleFilter
)
from vtkmodules.vtkFiltersGeneral import vtkBooleanOperationPolyDataFilter
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkIOGeometry import (
    vtkBYUReader,
    vtkOBJReader,
    vtkSTLReader
)
from vtkmodules.vtkIOLegacy import vtkPolyDataReader
from vtkmodules.vtkIOPLY import vtkPLYReader
from vtkmodules.vtkIOXML import vtkXMLPolyDataReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def get_program_parameters():
    import argparse
    description = 'How to align two vtkPolyData\'s.'
    epilogue = '''

    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('-o', '--operation', nargs='?', default='intersection', help='The type of  ')
    parser.add_argument('fn1', nargs='?', default=None, help='The polydata source file name,e.g. Grey_Nurse_Shark.stl.')
    parser.add_argument('fn2', nargs='?', default=None, help='The polydata target file name, e.g. shark.ply.')

    args = parser.parse_args()

    return args.operation, args.fn1, args.fn2


def main():
    colors = vtkNamedColors()

    operation, fn1, fn2 = get_program_parameters()
    if fn1 and fn2:
        poly1 = ReadPolyData(fn1)
        tri1 = vtkTriangleFilter()
        tri1.SetInputData(poly1)
        clean1 = vtkCleanPolyData()
        clean1.SetInputConnection(tri1.GetOutputPort())
        clean1.Update()
        input1 = clean1.GetOutput()

        poly2 = ReadPolyData(fn2)
        tri2 = vtkTriangleFilter()
        tri2.SetInputData(poly2)
        tri2.Update()
        clean2 = vtkCleanPolyData()
        clean2.SetInputConnection(tri2.GetOutputPort())
        clean2.Update()
        input2 = clean2.GetOutput()
    else:
        sphereSource1 = vtkSphereSource()
        sphereSource1.SetCenter(0.25, 0, 0)
        sphereSource1.SetPhiResolution(21)
        sphereSource1.SetThetaResolution(21)
        sphereSource1.Update()
        input1 = sphereSource1.GetOutput()

        sphereSource2 = vtkSphereSource()
        sphereSource2.Update()
        input2 = sphereSource2.GetOutput()

    input1Mapper = vtkPolyDataMapper()
    input1Mapper.SetInputData(input1)
    input1Mapper.ScalarVisibilityOff()
    input1Actor = vtkActor()
    input1Actor.SetMapper(input1Mapper)
    input1Actor.GetProperty().SetDiffuseColor(colors.GetColor3d('Tomato'))
    input1Actor.GetProperty().SetSpecular(0.6)
    input1Actor.GetProperty().SetSpecularPower(20)
    input1Actor.SetPosition(input1.GetBounds()[1] - input1.GetBounds()[0], 0, 0)

    input2Mapper = vtkPolyDataMapper()
    input2Mapper.SetInputData(input2)
    input2Mapper.ScalarVisibilityOff()
    input2Actor = vtkActor()
    input2Actor.SetMapper(input2Mapper)
    input2Actor.GetProperty().SetDiffuseColor(colors.GetColor3d('Mint'))
    input2Actor.GetProperty().SetSpecular(0.6)
    input2Actor.GetProperty().SetSpecularPower(20)
    input2Actor.SetPosition(-(input1.GetBounds()[1] - input1.GetBounds()[0]), 0, 0)

    booleanOperation = vtkBooleanOperationPolyDataFilter()
    if operation.lower() == 'union':
        booleanOperation.SetOperationToUnion()
    elif operation.lower() == 'intersection':
        booleanOperation.SetOperationToIntersection()
    elif operation.lower() == 'difference':
        booleanOperation.SetOperationToDifference()
    else:
        print('Unknown operation:', operation)
        return

    booleanOperation.SetInputData(0, input1)
    booleanOperation.SetInputData(1, input2)

    booleanOperationMapper = vtkPolyDataMapper()
    booleanOperationMapper.SetInputConnection(booleanOperation.GetOutputPort())
    booleanOperationMapper.ScalarVisibilityOff()

    booleanOperationActor = vtkActor()
    booleanOperationActor.SetMapper(booleanOperationMapper)
    booleanOperationActor.GetProperty().SetDiffuseColor(colors.GetColor3d('Banana'))
    booleanOperationActor.GetProperty().SetSpecular(0.6)
    booleanOperationActor.GetProperty().SetSpecularPower(20)

    renderer = vtkRenderer()
    renderer.AddViewProp(input1Actor)
    renderer.AddViewProp(input2Actor)
    renderer.AddViewProp(booleanOperationActor)
    renderer.SetBackground(colors.GetColor3d('Silver'))
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetSize(640, 480)
    renderWindow.SetWindowName('BooleanOperationPolyDataFilter')

    viewUp = [0.0, 0.0, 1.0]
    position = [0.0, -1.0, 0.0]
    PositionCamera(renderer, viewUp, position)
    renderer.GetActiveCamera().Dolly(1.4)
    renderer.ResetCameraClippingRange()

    renWinInteractor = vtkRenderWindowInteractor()
    renWinInteractor.SetRenderWindow(renderWindow)

    renderWindow.Render()
    renWinInteractor.Start()


def ReadPolyData(file_name):
    import os
    path, extension = os.path.splitext(file_name)
    extension = extension.lower()
    if extension == '.ply':
        reader = vtkPLYReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == '.vtp':
        reader = vtkXMLPolyDataReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == '.obj':
        reader = vtkOBJReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == '.stl':
        reader = vtkSTLReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == '.vtk':
        reader = vtkPolyDataReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == '.g':
        reader = vtkBYUReader()
        reader.SetGeometryFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    else:
        # Return a None if the extension is unknown.
        poly_data = None
    return poly_data


def PositionCamera(renderer, viewUp, position):
    renderer.GetActiveCamera().SetViewUp(viewUp)
    renderer.GetActiveCamera().SetPosition(position)
    renderer.ResetCamera()


if __name__ == '__main__':
    main()
