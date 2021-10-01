#!/usr/bin/env python

import vtkmodules.all as vtk


def main():
    xyzFile, qFile = get_program_parameters()

    colors = vtk.vtkNamedColors()

    ren1 = vtk.vtkRenderer()

    renWin = vtk.vtkRenderWindow()
    renWin.AddRenderer(ren1)

    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # The cut data.
    pl3d = vtk.vtkMultiBlockPLOT3DReader()
    pl3d.SetXYZFileName(xyzFile)
    pl3d.SetQFileName(qFile)
    pl3d.SetScalarFunctionNumber(100)
    pl3d.SetVectorFunctionNumber(202)
    pl3d.Update()

    sg = pl3d.GetOutput().GetBlock(0)

    plane = vtk.vtkPlane()
    plane.SetOrigin(sg.GetCenter())
    plane.SetNormal(-0.287, 0, 0.9579)

    planeCut = vtk.vtkCutter()
    planeCut.SetInputData(pl3d.GetOutput().GetBlock(0))
    planeCut.SetCutFunction(plane)

    cutMapper = vtk.vtkDataSetMapper()
    cutMapper.SetInputConnection(planeCut.GetOutputPort())
    cutMapper.SetScalarRange(sg.GetPointData().GetScalars().GetRange())

    cutActor = vtk.vtkActor()
    cutActor.SetMapper(cutMapper)

    # Extract the plane.
    compPlane = vtk.vtkStructuredGridGeometryFilter()
    compPlane.SetInputData(sg)
    compPlane.SetExtent(0, 100, 0, 100, 9, 9)

    planeMapper = vtk.vtkPolyDataMapper()
    planeMapper.SetInputConnection(compPlane.GetOutputPort())
    planeMapper.ScalarVisibilityOff()

    planeActor = vtk.vtkActor()
    planeActor.SetMapper(planeMapper)
    planeActor.GetProperty().SetRepresentationToWireframe()
    planeActor.GetProperty().SetColor(colors.GetColor3d('Wheat'))

    # Outline.
    outline = vtk.vtkStructuredGridOutlineFilter()
    outline.SetInputData(pl3d.GetOutput().GetBlock(0))

    outlineMapper = vtk.vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())

    outlineActor = vtk.vtkActor()
    outlineActor.SetMapper(outlineMapper)
    outlineActor.GetProperty().SetColor(colors.GetColor3d('Wheat'))

    # Add the actors to the renderer, set the background and size.
    #
    ren1.AddActor(outlineActor)
    ren1.AddActor(planeActor)
    ren1.AddActor(cutActor)
    ren1.SetBackground(colors.GetColor3d('SlateGray'))

    renWin.SetSize(640, 480)
    renWin.SetWindowName('CutStructuredGrid')

    camera = ren1.GetActiveCamera()
    camera.SetPosition(5.02611, -23.535, 50.3979)
    camera.SetFocalPoint(9.33614, 0.0414149, 30.112)
    camera.SetViewUp(-0.0676794, 0.657814, 0.750134)
    camera.SetDistance(31.3997)
    camera.SetClippingRange(12.1468, 55.8147)

    # Render the image.
    #
    renWin.Render()
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'Cut through structured grid with plane.'
    epilogue = '''
    The cut plane is shown solid shaded.
    A computational plane of constant k value is shown in wireframe for comparison.
    The colors correspond to flow density.
    Cutting surfaces are not necessarily planes: implicit functions such as spheres, cylinders,
     and quadrics can also be used.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename1', help='combxyz.bin.')
    parser.add_argument('filename2', help='combq.bin.')
    args = parser.parse_args()
    return args.filename1, args.filename2


if __name__ == '__main__':
    main()
