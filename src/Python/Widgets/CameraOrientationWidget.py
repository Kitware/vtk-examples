#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This example demonstrates how to use the vtkCameraOrientationWidget to control
# a renderer's camera orientation.

from pathlib import Path

import vtkmodules.all as vtk


def get_program_parameters():
    import argparse
    description = 'Demonstrates a 3D camera orientation widget.'
    epilogue = '''
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('path', help='The path to the file to render e.g. cow.vtp.')
    args = parser.parse_args()
    return args.path


def main():
    colors = vtk.vtkNamedColors()

    path = get_program_parameters()
    if not Path(path).is_file():
        print('Unable to find the file:', path)
        return

    renderer = vtk.vtkRenderer()
    ren_win = vtk.vtkRenderWindow()
    interactor = vtk.vtkRenderWindowInteractor()

    reader = vtk.vtkXMLPolyDataReader()
    reader.SetFileName(path)

    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(reader.GetOutputPort())

    actor = vtk.vtkActor()
    actor.GetProperty().SetColor(colors.GetColor3d('Beige'))
    actor.SetMapper(mapper)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('DimGray'))

    ren_win.AddRenderer(renderer)
    ren_win.SetSize(600, 600)
    ren_win.SetWindowName('CameraOrientationWidget')

    # Important: The interactor must be set prior to enabling the widget.
    interactor.SetRenderWindow(ren_win)

    cam_orient_manipulator = vtk.vtkCameraOrientationWidget()
    cam_orient_manipulator.SetParentRenderer(renderer)
    # Enable the widget.
    cam_orient_manipulator.On()

    ren_win.Render()
    interactor.Initialize()
    interactor.Start()


if __name__ == "__main__":
    main()
