#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkContourFilter
from vtkmodules.vtkFiltersModeling import vtkOutlineFilter
from vtkmodules.vtkIOImage import vtkMetaImageReader
from vtkmodules.vtkImagingCore import vtkExtractVOI
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    fileName = get_program_parameters()

    colors = vtkNamedColors()

    # Create the RenderWindow, Renderer and Interactor.
    #

    ren1 = vtkRenderer()

    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren1)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Create the pipeline.
    #

    reader = vtkMetaImageReader()
    reader.SetFileName(fileName)
    reader.Update()

    extractVOI = vtkExtractVOI()
    extractVOI.SetInputConnection(reader.GetOutputPort())
    extractVOI.SetVOI(0, 255, 0, 255, 45, 45)

    iso = vtkContourFilter()
    iso.SetInputConnection(extractVOI.GetOutputPort())
    iso.GenerateValues(12, 500, 1150)

    isoMapper = vtkPolyDataMapper()
    isoMapper.SetInputConnection(iso.GetOutputPort())
    isoMapper.ScalarVisibilityOff()

    isoActor = vtkActor()
    isoActor.SetMapper(isoMapper)
    isoActor.GetProperty().SetColor(colors.GetColor3d('Wheat'))

    outline = vtkOutlineFilter()
    outline.SetInputConnection(extractVOI.GetOutputPort())

    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())

    outlineActor = vtkActor()
    outlineActor.SetMapper(outlineMapper)

    # Add the actors to the renderer, set the background and size.
    #
    ren1.AddActor(outlineActor)
    ren1.AddActor(isoActor)
    ren1.SetBackground(colors.GetColor3d('SlateGray'))
    ren1.ResetCamera()
    ren1.GetActiveCamera().Dolly(1.5)
    ren1.ResetCameraClippingRange()

    renWin.SetSize(640, 640)
    renWin.SetWindowName('HeadSlice')

    renWin.Render()
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'Marching squares are used to generate contour lines.'
    epilogue = '''
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='FullHead.mhd.')
    args = parser.parse_args()
    return args.filename


if __name__ == '__main__':
    main()
