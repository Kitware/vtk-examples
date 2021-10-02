#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkIOGeometry import (
    vtkSTLReader,
    vtkSTLWriter
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def get_program_parameters():
    import argparse
    description = 'Generate image data, then write a .stl file.'
    epilogue = '''
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename', help='A required stl filename.', nargs='?',
                        const='TestWriteSTL.ply',
                        type=str, default='TestWriteSTL.ply')
    args = parser.parse_args()
    return args.filename


def main():
    colors = vtkNamedColors()

    filename = get_program_parameters()

    sphereSource = vtkSphereSource()
    sphereSource.Update()

    # Write the stl file to disk
    stlWriter = vtkSTLWriter()
    stlWriter.SetFileName(filename)
    stlWriter.SetInputConnection(sphereSource.GetOutputPort())
    stlWriter.Write()

    # Read and display for verification
    reader = vtkSTLReader()
    reader.SetFileName(filename)

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(reader.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    # Create a rendering window and renderer
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)
    renWin.SetWindowName('WriteSTL')

    # Create a renderwindowinteractor
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Assign actor to the renderer
    ren.AddActor(actor)
    ren.SetBackground(colors.GetColor3d('cobalt_green'))

    # Enable user interface interactor
    iren.Initialize()
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
