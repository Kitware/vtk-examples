#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkPlane
from vtkmodules.vtkFiltersCore import (
    vtkCutter,
    vtkStructuredGridOutlineFilter
)
from vtkmodules.vtkFiltersGeometry import vtkStructuredGridGeometryFilter
from vtkmodules.vtkIOParallel import vtkMultiBlockPLOT3DReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    xyzFile, qFile = get_program_parameters()

    colors = vtkNamedColors()

    ren1 = vtkRenderer()

    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren1)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # The cut data.
    pl3d = vtkMultiBlockPLOT3DReader()
    pl3d.SetXYZFileName(xyzFile)
    pl3d.SetQFileName(qFile)
    pl3d.SetScalarFunctionNumber(100)
    pl3d.SetVectorFunctionNumber(202)
    pl3d.Update()

    sg = pl3d.GetOutput().GetBlock(0)

    plane = vtkPlane()
    plane.SetOrigin(sg.GetCenter())
    plane.SetNormal(-0.287, 0, 0.9579)

    planeCut = vtkCutter()
    planeCut.SetInputData(pl3d.GetOutput().GetBlock(0))
    planeCut.SetCutFunction(plane)

    cutMapper = vtkDataSetMapper()
    cutMapper.SetInputConnection(planeCut.GetOutputPort())
    cutMapper.SetScalarRange(sg.GetPointData().GetScalars().GetRange())

    cutActor = vtkActor()
    cutActor.SetMapper(cutMapper)

    # Extract the plane.
    compPlane = vtkStructuredGridGeometryFilter()
    compPlane.SetInputData(sg)
    compPlane.SetExtent(0, 100, 0, 100, 9, 9)

    planeMapper = vtkPolyDataMapper()
    planeMapper.SetInputConnection(compPlane.GetOutputPort())
    planeMapper.ScalarVisibilityOff()

    planeActor = vtkActor()
    planeActor.SetMapper(planeMapper)
    planeActor.GetProperty().SetRepresentationToWireframe()
    planeActor.GetProperty().SetColor(colors.GetColor3d('Wheat'))

    # Outline.
    outline = vtkStructuredGridOutlineFilter()
    outline.SetInputData(pl3d.GetOutput().GetBlock(0))

    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())

    outlineActor = vtkActor()
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
