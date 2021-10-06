# Translated from vib.tcl

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import (
    vtkPolyDataNormals,
    vtkVectorDot
)
from vtkmodules.vtkFiltersGeneral import vtkWarpVector
from vtkmodules.vtkFiltersModeling import vtkOutlineFilter
from vtkmodules.vtkIOLegacy import vtkPolyDataReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    file_name = get_program_parameters()

    colors = vtkNamedColors()

    # Set the colors.
    colors.SetColor('PlateColor', [255, 160, 140, 255])
    colors.SetColor('BkgColor', [65, 99, 149, 255])

    # Read a vtk file
    #
    plate = vtkPolyDataReader()
    plate.SetFileName(file_name)
    plate.Update()
    bounds = [0] * 6
    plate.GetOutput().GetBounds(bounds)
    plate.SetVectorsName('mode2')

    normals = vtkPolyDataNormals()
    normals.SetInputConnection(plate.GetOutputPort())
    warp = vtkWarpVector()
    warp.SetInputConnection(normals.GetOutputPort())
    warp.SetScaleFactor(0.5)
    color = vtkVectorDot()
    color.SetInputConnection(warp.GetOutputPort())
    plateMapper = vtkDataSetMapper()
    plateMapper.SetInputConnection(warp.GetOutputPort())
    plateActor = vtkActor()
    plateActor.SetMapper(plateMapper)
    plateActor.GetProperty().SetColor(
        colors.GetColor3d('PlateColor'))
    plateActor.RotateX(-90)

    # Create the outline.
    #
    outline = vtkOutlineFilter()
    outline.SetInputConnection(plate.GetOutputPort())
    spikeMapper = vtkPolyDataMapper()
    spikeMapper.SetInputConnection(outline.GetOutputPort())
    outlineActor = vtkActor()
    outlineActor.SetMapper(spikeMapper)
    outlineActor.RotateX(-90)
    outlineActor.GetProperty().SetColor(colors.GetColor3d('White'))

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
    ren.AddActor(outlineActor)
    renWin.SetSize(500, 500)
    renWin.SetWindowName('PlateVibration')

    # Render the image.
    renWin.Render()
    ren.SetBackground(colors.GetColor3d('BkgColor'))
    # This closely matches the original illustration.
    ren.GetActiveCamera().SetPosition(-3.7, 13, 15.5)
    ren.ResetCameraClippingRange()

    renWin.Render()

    iren.Start()


def get_program_parameters():
    import argparse
    description = 'Produces figure 6-14(a) Beam displacement from the VTK Textbook.'
    epilogue = '''
        Produce figure 6–14(a) Beam displacement from the VTK Textbook..
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename', help='plate.vtk')
    args = parser.parse_args()
    return args.filename


if __name__ == '__main__':
    main()
