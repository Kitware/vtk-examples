#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersGeneral import vtkAxes
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkIOGeometry import (
    vtkBYUReader,
    vtkOBJReader,
    vtkSTLReader
)
from vtkmodules.vtkIOPLY import vtkPLYReader
from vtkmodules.vtkIOXML import vtkXMLPolyDataReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    '''
        To match the illustrations in VTKTextbook.pdf, use BkgColor as the background and
          Wheat as the cow colour.
        Also comment out the lines:
          modelActor->GetProperty()->SetSpecular(.6);
          modelActor->GetProperty()->SetSpecularPower(30);
        and use cow.g as the input data.

    '''
    file_name, figure, actor_color = get_program_parameters()
    rotate(file_name, figure, actor_color)


def rotate(file_name, figure, actor_color):
    ''''
    This is where we do the rotations.

    '''
    # Create renderer stuff
    #
    colors = vtkNamedColors()
    # Set the background color.
    colors.SetColor('BkgColor', [26, 51, 102, 255])
    # colors.SetColor('BkgColor', [60, 93, 144, 255])

    ren1 = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren1)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Create the pipeline.
    #
    polyData = ReadPolyData(file_name)

    modelMapper = vtkPolyDataMapper()
    modelMapper.SetInputData(polyData)

    modelActor = vtkActor()
    modelActor.SetMapper(modelMapper)
    modelActor.GetProperty().SetDiffuseColor(colors.GetColor3d(actor_color))
    if actor_color != 'Wheat':
        modelActor.GetProperty().SetSpecular(0.6)
        modelActor.GetProperty().SetSpecularPower(30)

    modelAxesSource = vtkAxes()
    modelAxesSource.SetScaleFactor(10)
    modelAxesSource.SetOrigin(0, 0, 0)

    modelAxesMapper = vtkPolyDataMapper()
    modelAxesMapper.SetInputConnection(modelAxesSource.GetOutputPort())

    modelAxes = vtkActor()
    modelAxes.SetMapper(modelAxesMapper)

    ren1.AddActor(modelAxes)
    modelAxes.VisibilityOff()

    # Add the actors to the renderer, set the background and size.
    #
    ren1.AddActor(modelActor)
    if actor_color == 'Wheat':
        ren1.SetBackground(colors.GetColor3d('BkgColor'))
    else:
        ren1.SetBackground(colors.GetColor3d('Silver'))
    renWin.SetSize(640, 480)
    renWin.SetWindowName('Rotations')

    ren1.ResetCamera()
    ren1.GetActiveCamera().Azimuth(0)
    ren1.GetActiveCamera().SetClippingRange(0.1, 1000.0)

    modelAxes.VisibilityOn()

    renWin.Render()
    renWin.Render()

    if figure == 1:
        RotateX(renWin, modelActor)
    elif figure == 2:
        RotateY(renWin, modelActor)
    elif figure == 3:
        RotateZ(renWin, modelActor)
    else:
        RotateXY(renWin, modelActor)

    renWin.EraseOff()
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'Perform rotations about the X, Y, Z and X then Y axes.'
    epilogue = '''
        Perform rotations about the X, Y, Z and X then Y axes.
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename', help='The file cow.obj.')
    parser.add_argument('figure', default=0, type=int, nargs='?',
                        help='The particular rotation that you want to view.')
    parser.add_argument('actor_color', default='Wheat', type=str, nargs='?',
                        help='If the color is Wheat then the vtk textbook colors are used.')
    args = parser.parse_args()
    return args.filename, args.figure, args.actor_color


def RotateX(renWin, actor):
    actor.SetOrientation(0, 0, 0)
    renWin.Render()
    renWin.Render()
    renWin.EraseOff()

    for i in range(0, 6):
        actor.RotateX(60)
        renWin.Render()
        renWin.Render()
    renWin.EraseOn()


def RotateY(renWin, actor):
    actor.SetOrientation(0, 0, 0)
    renWin.Render()
    renWin.EraseOff()

    for i in range(0, 6):
        actor.RotateY(60)
        renWin.Render()
        renWin.Render()
    renWin.EraseOn()


def RotateZ(renWin, actor):
    actor.SetOrientation(0, 0, 0)
    renWin.Render()
    renWin.EraseOff()

    for i in range(0, 6):
        actor.RotateZ(60)
        renWin.Render()
        renWin.Render()
    renWin.EraseOn()


def RotateXY(renWin, actor):
    actor.SetOrientation(0, 0, 0)
    actor.RotateX(60)
    renWin.Render()
    renWin.Render()
    renWin.EraseOff()

    for i in range(0, 6):
        actor.RotateY(60)
        renWin.Render()
        renWin.Render()
    renWin.EraseOn()


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
        reader = vtkXMLPolyDataReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == '.g':
        reader = vtkBYUReader()
        reader.SetGeometryFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    else:
        # Return a sphere if the extension is unknown.
        source = vtkSphereSource()
        source.Update()
        poly_data = source.GetOutput()
    return poly_data


if __name__ == '__main__':
    main()
