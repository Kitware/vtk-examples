#!/usr/bin/env python

##
# This example shows how to apply an vtkImageData texture to an sphere
# vtkPolyData object.
# Note: Input jpg file can be located in the VTKData repository.
#
# @author JBallesteros
##

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkFiltersTexture import vtkTextureMapToSphere
from vtkmodules.vtkIOImage import vtkJPEGReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTexture
)


def get_program_parameters():
    import argparse
    description = 'Texture an object with an image.'
    epilogue = '''
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='masonry-wide.jpg.')
    args = parser.parse_args()
    return args.filename


def main():
    colors = vtkNamedColors()

    jpegfile = get_program_parameters()

    # Create a render window
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)
    renWin.SetSize(480, 480)
    renWin.SetWindowName('SphereTexture')

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Generate an sphere polydata
    sphere = vtkSphereSource()
    sphere.SetThetaResolution(12)
    sphere.SetPhiResolution(12)

    # Read the image data from a file
    reader = vtkJPEGReader()
    reader.SetFileName(jpegfile)

    # Create texture object
    texture = vtkTexture()
    texture.SetInputConnection(reader.GetOutputPort())

    # Map texture coordinates
    map_to_sphere = vtkTextureMapToSphere()
    map_to_sphere.SetInputConnection(sphere.GetOutputPort())
    map_to_sphere.PreventSeamOn()

    # Create mapper and set the mapped texture as input
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(map_to_sphere.GetOutputPort())

    # Create actor and set the mapper and the texture
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.SetTexture(texture)

    ren.AddActor(actor)
    ren.SetBackground(colors.GetColor3d('RosyBrown'))

    iren.Initialize()
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
