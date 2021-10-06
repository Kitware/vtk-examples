#!/usr/bin/env python

# Translated from dispPlot.tcl

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkLookupTable
from vtkmodules.vtkFiltersCore import (
    vtkPolyDataNormals,
    vtkVectorDot
)
from vtkmodules.vtkFiltersGeneral import vtkWarpVector
from vtkmodules.vtkIOLegacy import vtkPolyDataReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkColorTransferFunction,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    file_name, color_scheme = get_program_parameters()

    color_scheme = abs(color_scheme)
    if color_scheme > 2:
        color_scheme = 0

    colors = vtkNamedColors()

    # Read a vtk file
    #
    plate = vtkPolyDataReader()
    plate.SetFileName(file_name)
    plate.SetVectorsName("mode8")
    plate.Update()

    warp = vtkWarpVector()
    warp.SetInputConnection(plate.GetOutputPort())
    warp.SetScaleFactor(0.5)

    normals = vtkPolyDataNormals()
    normals.SetInputConnection(warp.GetOutputPort())

    color = vtkVectorDot()
    color.SetInputConnection(normals.GetOutputPort())

    lut = vtkLookupTable()
    MakeLUT(color_scheme, lut)

    plateMapper = vtkDataSetMapper()
    plateMapper.SetInputConnection(color.GetOutputPort())
    plateMapper.SetLookupTable(lut)
    plateMapper.SetScalarRange(-1, 1)

    plateActor = vtkActor()
    plateActor.SetMapper(plateMapper)

    # Create the RenderWindow, Renderer and both Actors
    #
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Add the actors to the renderer, set the background and size
    #
    ren.AddActor(plateActor)
    ren.SetBackground(colors.GetColor3d("Wheat"))
    renWin.SetSize(512, 512)
    renWin.SetWindowName('DisplacementPlot')

    ren.GetActiveCamera().SetPosition(13.3991, 14.0764, 9.97787)
    ren.GetActiveCamera().SetFocalPoint(1.50437, 0.481517, 4.52992)
    ren.GetActiveCamera().SetViewAngle(30)
    ren.GetActiveCamera().SetViewUp(- 0.120861, 0.458556, - 0.880408)
    ren.GetActiveCamera().SetClippingRange(12.5724, 26.8374)
    # Render the image.
    renWin.Render()

    iren.Start()


def get_program_parameters():
    import argparse
    description = 'Produce figure 6–15(b) from the VTK Textbook.'
    epilogue = '''
        Produce figure 6–15(b) from the VTK Textbook.
        Surface plot of a vibrating plane.

        The color_scheme option allows you to select a series of colour schemes.
        0: The default:- cool maximum negative motion, warm maximum positive motion, white at the nodes.
        1: An alternative:- green maximum negative motion, purple maximum positive motion, white at the nodes.
        2: The original:- white at maximum motion, black at the nodes.

   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename', help='plate.vtk')
    parser.add_argument('color_scheme', default=0, type=int, nargs='?', help='The particular color scheme to use.')
    args = parser.parse_args()
    return args.filename, args.color_scheme


def MakeLUT(colorScheme, lut):
    # See: [Diverging Color Maps for Scientific Visualization]
    #      (http:#www.kennethmoreland.com/color-maps/)
    nc = 256
    ctf = vtkColorTransferFunction()

    if colorScheme == 1:
        # Green to purple diverging.
        ctf.SetColorSpaceToDiverging()
        ctf.AddRGBPoint(0.0, 0.085, 0.532, 0.201)
        ctf.AddRGBPoint(0.5, 0.865, 0.865, 0.865)
        ctf.AddRGBPoint(1.0, 0.436, 0.308, 0.631)
        lut.SetNumberOfTableValues(nc)
        lut.Build()
        for i in range(0, nc):
            rgb = list(ctf.GetColor(float(i) / nc))
            rgb.append(1.0)
            lut.SetTableValue(i, *rgb)
    elif colorScheme == 2:
        # Make a lookup table, black in the centre with bright areas
        #   at the beginning and end of the table.
        # This is from the original code.
        nc2 = nc / 2.0
        lut.SetNumberOfColors(nc)
        lut.Build()
        for i in range(0, int(nc2)):
            # White to black.
            v = (nc2 - i) / nc2
            lut.SetTableValue(i, v, v, v, 1)
        for i in range(int(nc2), nc):
            # Black to white.
            v = (i - nc2) / nc2
            lut.SetTableValue(i, v, v, v, 1)
    else:
        # Cool to warm diverging.
        ctf.SetColorSpaceToDiverging()
        ctf.AddRGBPoint(0.0, 0.230, 0.299, 0.754)
        ctf.AddRGBPoint(1.0, 0.706, 0.016, 0.150)
        lut.SetNumberOfTableValues(nc)
        lut.Build()
        for i in range(0, nc):
            rgb = list(ctf.GetColor(float(i) / nc))
            rgb.append(1.0)
            lut.SetTableValue(i, *rgb)


if __name__ == '__main__':
    main()
