#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkLookupTable
from vtkmodules.vtkFiltersCore import vtkHedgeHog
from vtkmodules.vtkFiltersModeling import vtkOutlineFilter
from vtkmodules.vtkIOLegacy import vtkStructuredPointsReader
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

    reader = vtkStructuredPointsReader()
    reader.SetFileName(fileName)

    hhog = vtkHedgeHog()
    hhog.SetInputConnection(reader.GetOutputPort())
    hhog.SetScaleFactor(0.3)

    lut = vtkLookupTable()
    # lut.SetHueRange(.667, 0.0)
    lut.Build()

    hhogMapper = vtkPolyDataMapper()
    hhogMapper.SetInputConnection(hhog.GetOutputPort())
    hhogMapper.SetScalarRange(50, 550)
    hhogMapper.SetLookupTable(lut)

    hhogActor = vtkActor()
    hhogActor.SetMapper(hhogMapper)

    outline = vtkOutlineFilter()
    outline.SetInputConnection(reader.GetOutputPort())

    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())

    outlineActor = vtkActor()
    outlineActor.SetMapper(outlineMapper)
    outlineActor.GetProperty().SetColor(colors.GetColor3d('Black'))

    aRenderer = vtkRenderer()
    aRenderWindow = vtkRenderWindow()
    aRenderWindow.AddRenderer(aRenderer)
    anInteractor = vtkRenderWindowInteractor()
    anInteractor.SetRenderWindow(aRenderWindow)
    aRenderWindow.SetSize(640, 480)
    aRenderWindow.SetWindowName('ComplexV')

    aRenderer.AddActor(outlineActor)
    aRenderer.AddActor(hhogActor)

    aRenderer.SetBackground(colors.GetColor3d('SlateGray'))

    # Generate an interesting view.

    aRenderer.GetActiveCamera().SetFocalPoint(0, 0, 0)
    aRenderer.GetActiveCamera().SetPosition(1, 0, 0)
    aRenderer.GetActiveCamera().SetViewUp(0, 0, 1)
    aRenderer.ResetCamera()

    aRenderer.GetActiveCamera().Azimuth(60)
    aRenderer.GetActiveCamera().Elevation(30)
    aRenderer.GetActiveCamera().Dolly(1.1)
    aRenderer.ResetCameraClippingRange()

    aRenderWindow.Render()

    # Interact with the data.
    anInteractor.Start()


def get_program_parameters():
    import argparse
    description = 'Vector visualization techniques.'
    epilogue = '''
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='carotid.vtk')
    args = parser.parse_args()
    return args.filename


if __name__ == '__main__':
    main()
