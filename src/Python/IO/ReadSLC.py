#!/usr/bin/env python

import vtk


def main():
    InputFilename, iso_value = get_program_parameters()

    colors = vtk.vtkNamedColors()

    # vtkSLCReader to read.
    reader = vtk.vtkSLCReader()
    reader.SetFileName(InputFilename)
    reader.Update()

    # Create a mapper.
    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(reader.GetOutputPort())

    # Implementing Marching Cubes Algorithm to create the surface using vtkContourFilter object.
    contourFilter = vtk.vtkContourFilter()
    contourFilter.SetInputConnection(reader.GetOutputPort())
    # Change the range(2nd and 3rd Paramater) based on your
    # requirement. recomended value for 1st parameter is above 1
    # contourFilter.GenerateValues(5, 80.0, 100.0)
    contourFilter.SetValue(0, iso_value)

    outliner = vtk.vtkOutlineFilter()
    outliner.SetInputConnection(reader.GetOutputPort())
    outliner.Update()

    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(contourFilter.GetOutputPort())
    mapper.SetScalarVisibility(0)

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetDiffuse(0.8)
    actor.GetProperty().SetDiffuseColor(colors.GetColor3d('Ivory'))
    actor.GetProperty().SetSpecular(0.8)
    actor.GetProperty().SetSpecularPower(120.0)

    # Create a rendering window and renderer.
    renderer = vtk.vtkRenderer()
    renderWindow = vtk.vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetSize(500, 500)

    # Create a renderwindowinteractor.
    renderWindowInteractor = vtk.vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Assign actor to the renderer.
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))

    # Pick a good view
    cam1 = renderer.GetActiveCamera()
    cam1.SetFocalPoint(0.0, 0.0, 0.0)
    cam1.SetPosition(0.0, -1.0, 0.0)
    cam1.SetViewUp(0.0, 0.0, -1.0)
    cam1.Azimuth(-90.0)
    renderer.ResetCamera()
    renderer.ResetCameraClippingRange()

    renderWindow.SetWindowName('ReadSLC')
    renderWindow.SetSize(640, 512)
    renderWindow.Render()

    # Enable user interface interactor.
    renderWindowInteractor.Initialize()
    renderWindow.Render()
    renderWindowInteractor.Start()


def get_program_parameters():
    import argparse
    description = 'Read a .slc file.'
    epilogue = ''''''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='vw_knee.slc.')
    parser.add_argument('iso_value', nargs='?', type=float, default=72.0, help='Defaullt 72.')
    args = parser.parse_args()
    return args.filename, args.iso_value


if __name__ == '__main__':
    main()
