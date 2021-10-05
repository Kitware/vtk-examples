#!/usr/bin/env python


import os

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersCore import vtkTubeFilter
from vtkmodules.vtkFiltersGeneral import vtkTransformPolyDataFilter
from vtkmodules.vtkFiltersModeling import (
    vtkLinearExtrusionFilter,
    vtkRibbonFilter
)
from vtkmodules.vtkIOLegacy import vtkPolyDataReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkFollower,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)
from vtkmodules.vtkRenderingFreeType import vtkVectorText


def main():
    colors = vtkNamedColors()

    fileNames, useRibbons = get_program_parameters()
    useTubes = not useRibbons

    # Set up the stocks
    renderers = list()
    topRenderer = vtkRenderer()
    bottomRenderer = vtkRenderer()
    renderers.append(topRenderer)
    renderers.append(bottomRenderer)

    zPosition = 0.0
    for fn in fileNames:
        zPosition = AddStock(renderers, fn, os.path.basename((os.path.splitext(fn)[0])), zPosition, useTubes)

    # Setup the render window and interactor.
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderers[0])
    renderWindow.AddRenderer(renderers[1])

    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderers[0].SetViewport(0.0, 0.4, 1.0, 1.0)
    renderers[1].SetViewport(0.0, 0.0, 1.0, 0.4)

    renderers[0].GetActiveCamera().SetViewAngle(5.0)
    renderers[0].ResetCamera()
    renderers[0].GetActiveCamera().Zoom(1.4)
    renderers[0].ResetCameraClippingRange()
    renderers[0].SetBackground(colors.GetColor3d("SteelBlue"))

    renderers[1].GetActiveCamera().SetViewUp(0, 0, -1)
    renderers[1].GetActiveCamera().SetPosition(0, 1, 0)
    renderers[1].GetActiveCamera().SetViewAngle(5.0)
    renderers[1].ResetCamera()
    renderers[1].GetActiveCamera().Zoom(2.2)
    renderers[1].ResetCameraClippingRange()
    renderers[1].SetBackground(colors.GetColor3d("LightSteelBlue"))

    renderWindow.SetSize(500, 800)
    renderWindow.SetWindowName('Stocks')
    renderWindow.Render()

    renderWindowInteractor.Start()


def get_program_parameters():
    import argparse
    description = 'Two views from the stock visualization script.'
    epilogue = '''
    The top shows closing price over time; the bottom shows volume over time.  
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filenames', nargs='+',
                        help='List of one or more filenames corresponding to stocks. e.g. GE.vtk GM.vtk IBM.vtk DEC.vtk')
    parser.add_argument('-r', dest='useRibbons', action='store_true', help='Use ribbons instead of tubes.')

    args = parser.parse_args()
    return args.filenames, args.useRibbons


def AddStock(renderers, filename, name, zPosition, useTubes):
    print("Adding", name)

    # Read the data
    PolyDataRead = vtkPolyDataReader()
    PolyDataRead.SetFileName(filename)
    PolyDataRead.Update()

    # Create the labels.
    TextSrc = vtkVectorText()
    TextSrc.SetText(name)

    numberOfPoints = PolyDataRead.GetOutput().GetNumberOfPoints()

    nameIndex = int((numberOfPoints - 1) * 0.8)

    nameLocation = PolyDataRead.GetOutput().GetPoint(nameIndex)

    x = nameLocation[0] * 0.15
    y = nameLocation[1] + 5.0
    z = zPosition

    # Create a tube and ribbpn filter. One or the other will be used
    TubeFilter = vtkTubeFilter()
    TubeFilter.SetInputConnection(PolyDataRead.GetOutputPort())
    TubeFilter.SetNumberOfSides(8)
    TubeFilter.SetRadius(0.5)
    TubeFilter.SetRadiusFactor(10000)

    RibbonFilter = vtkRibbonFilter()
    RibbonFilter.SetInputConnection(PolyDataRead.GetOutputPort())
    RibbonFilter.VaryWidthOn()
    RibbonFilter.SetWidthFactor(5)
    RibbonFilter.SetDefaultNormal(0, 1, 0)
    RibbonFilter.UseDefaultNormalOn()

    Extrude = vtkLinearExtrusionFilter()
    Extrude.SetInputConnection(RibbonFilter.GetOutputPort())
    Extrude.SetVector(0, 1, 0)
    Extrude.SetExtrusionType(1)
    Extrude.SetScaleFactor(0.7)

    Transform = vtkTransform()
    Transform.Translate(0, 0, zPosition)
    Transform.Scale(0.15, 1, 1)

    TransformFilter = vtkTransformPolyDataFilter()
    TransformFilter.SetInputConnection(Extrude.GetOutputPort())
    TransformFilter.SetTransform(Transform)

    # Select tubes or ribbons
    if useTubes:
        TransformFilter.SetInputConnection(TubeFilter.GetOutputPort())
    else:
        TransformFilter.SetInputConnection(Extrude.GetOutputPort())

    for r in range(0, len(renderers)):
        LabelMapper = vtkPolyDataMapper()
        LabelMapper.SetInputConnection(TextSrc.GetOutputPort())

        LabelActor = vtkFollower()
        LabelActor.SetMapper(LabelMapper)
        LabelActor.SetPosition(x, y, z)
        LabelActor.SetScale(2, 2, 2)
        LabelActor.SetOrigin(TextSrc.GetOutput().GetCenter())

        # Increment zPosition.
        zPosition += 8.0

        StockMapper = vtkPolyDataMapper()
        StockMapper.SetInputConnection(TransformFilter.GetOutputPort())
        StockMapper.SetScalarRange(0, 8000)
        StockActor = vtkActor()
        StockActor.SetMapper(StockMapper)

        renderers[r].AddActor(StockActor)
        renderers[r].AddActor(LabelActor)
        LabelActor.SetCamera(renderers[r].GetActiveCamera())
    return zPosition


if __name__ == '__main__':
    main()
