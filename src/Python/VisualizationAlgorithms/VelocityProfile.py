#!/usr/bin/env python

# Translated from velProf.tcl.

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import (
    vtkAppendPolyData,
    vtkPolyDataNormals,
    vtkStructuredGridOutlineFilter
)
from vtkmodules.vtkFiltersGeneral import vtkWarpVector
from vtkmodules.vtkFiltersGeometry import vtkStructuredGridGeometryFilter
from vtkmodules.vtkIOParallel import vtkMultiBlockPLOT3DReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    fileName1, fileName2 = get_program_parameters()

    colors = vtkNamedColors()

    # Set the background color.
    colors.SetColor('BkgColor', [65, 99, 149, 255])

    # Read a vtk file
    #
    pl3d = vtkMultiBlockPLOT3DReader()
    pl3d.SetXYZFileName(fileName1)
    pl3d.SetQFileName(fileName2)
    pl3d.SetScalarFunctionNumber(100)  # Density
    pl3d.SetVectorFunctionNumber(202)  # Momentum
    pl3d.Update()

    pl3dOutput = pl3d.GetOutput().GetBlock(0)

    # What do we know about the data?
    # Get the extent of the data: imin,imax, jmin,jmax, kmin,kmax
    extent = pl3dOutput.GetExtent()
    scalarRange = pl3dOutput.GetScalarRange()

    # Planes are specified using a imin,imax, jmin,jmax, kmin,kmax coordinate
    # specification. Min and max i,j,k values are clamped to 0 and maximum value.
    # See the variable named extent for the values.
    #
    plane = vtkStructuredGridGeometryFilter()
    plane.SetInputData(pl3dOutput)
    plane.SetExtent(10, 10, 1, extent[3], 1, extent[5])

    plane2 = vtkStructuredGridGeometryFilter()
    plane2.SetInputData(pl3dOutput)
    plane2.SetExtent(30, 30, 1, extent[3], 1, extent[5])

    plane3 = vtkStructuredGridGeometryFilter()
    plane3.SetInputData(pl3dOutput)
    plane3.SetExtent(45, 45, 1, extent[3], 1, extent[5])

    # We use an append filter because that way we can do the warping, etc. just
    # using a single pipeline and actor.
    #
    appendF = vtkAppendPolyData()
    appendF.AddInputConnection(plane.GetOutputPort())
    appendF.AddInputConnection(plane2.GetOutputPort())
    appendF.AddInputConnection(plane3.GetOutputPort())

    # Warp
    warp = vtkWarpVector()
    warp.SetInputConnection(appendF.GetOutputPort())
    warp.SetScaleFactor(0.005)
    warp.Update()

    normals = vtkPolyDataNormals()
    normals.SetInputData(warp.GetPolyDataOutput())
    normals.SetFeatureAngle(45)

    planeMapper = vtkPolyDataMapper()
    planeMapper.SetInputConnection(normals.GetOutputPort())
    planeMapper.SetScalarRange(scalarRange)

    planeActor = vtkActor()
    planeActor.SetMapper(planeMapper)

    # The outline provides context for the data and the planes.
    outline = vtkStructuredGridOutlineFilter()
    outline.SetInputData(pl3dOutput)

    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())

    outlineActor = vtkActor()
    outlineActor.SetMapper(outlineMapper)
    outlineActor.GetProperty().SetColor(colors.GetColor3d('Black'))

    # Create the RenderWindow, Renderer and both Actors
    #
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Add the actors to the renderer, set the background and size
    #
    ren.AddActor(planeActor)
    ren.AddActor(outlineActor)
    ren.SetBackground(colors.GetColor3d('BkgColor'))

    renWin.SetSize(512, 512)
    renWin.SetWindowName('VelocityProfile')

    iren.Initialize()

    renWin.Render()

    ren.GetActiveCamera().SetPosition(19.8562, -31.8912, 47.0755)
    ren.GetActiveCamera().SetFocalPoint(8.255, 0.147815, 29.7631)
    ren.GetActiveCamera().SetViewUp(-0.0333325, 0.465756, 0.884285)
    ren.GetActiveCamera().SetClippingRange(17.3078, 64.6375)
    renWin.Render()

    iren.Start()


def get_program_parameters():
    import argparse
    description = 'Produce figure 6–14(b) Flow momentum from the VTK Textbook.'
    epilogue = '''
        Produce figure 6–14(b) Flow momentum from the VTK Textbook.

   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename1', help='combxyz.bin.')
    parser.add_argument('filename2', help='combq.bin.')
    args = parser.parse_args()
    return args.filename1, args.filename2


if __name__ == '__main__':
    main()
