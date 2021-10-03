#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import (
    vtkDataObjectToDataSetFilter,
    vtkFieldDataToAttributeDataFilter,
    vtkTubeFilter
)
from vtkmodules.vtkFiltersGeneral import (
    vtkAxes,
    vtkMarchingContourFilter
)
from vtkmodules.vtkIOLegacy import vtkDataObjectReader
from vtkmodules.vtkImagingHybrid import vtkGaussianSplatter
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkFollower,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)
from vtkmodules.vtkRenderingFreeType import vtkVectorText


def main():
    ifn = get_program_parameters()

    colors = vtkNamedColors()

    reader = vtkDataObjectReader()
    reader.SetFileName(ifn)

    size = 3187  # maximum number possible

    xAxis = 'INTEREST_RATE'
    yAxis = 'MONTHLY_PAYMENT'
    zAxis = 'MONTHLY_INCOME'
    scalar = 'TIME_LATE'

    # Extract data from field as a polydata (just points), then extract scalars.
    do2ds = vtkDataObjectToDataSetFilter()
    do2ds.SetInputConnection(reader.GetOutputPort())
    do2ds.SetDataSetTypeToPolyData()
    # format: component#, arrayname, arraycomp, minArrayId, maxArrayId, normalize
    do2ds.DefaultNormalizeOn()
    do2ds.SetPointComponent(0, xAxis, 0)
    do2ds.SetPointComponent(1, yAxis, 0, 0, size, 1)
    do2ds.SetPointComponent(2, zAxis, 0)
    do2ds.Update()
    fd2ad = vtkFieldDataToAttributeDataFilter()
    fd2ad.SetInputConnection(do2ds.GetOutputPort())
    fd2ad.SetInputFieldToDataObjectField()
    fd2ad.SetOutputAttributeDataToPointData()
    fd2ad.DefaultNormalizeOn()
    fd2ad.SetScalarComponent(0, scalar, 0)

    # Construct the pipeline for the original population.
    popSplatter = vtkGaussianSplatter()
    popSplatter.SetInputConnection(fd2ad.GetOutputPort())
    popSplatter.SetSampleDimensions(150, 150, 150)
    popSplatter.SetRadius(0.05)
    popSplatter.ScalarWarpingOff()

    popSurface = vtkMarchingContourFilter()
    popSurface.SetInputConnection(popSplatter.GetOutputPort())
    popSurface.SetValue(0, 0.01)
    popMapper = vtkPolyDataMapper()
    popMapper.SetInputConnection(popSurface.GetOutputPort())
    popMapper.ScalarVisibilityOff()
    popActor = vtkActor()
    popActor.SetMapper(popMapper)
    popActor.GetProperty().SetOpacity(0.3)
    popActor.GetProperty().SetColor(colors.GetColor3d('Gold'))

    # Construct the pipeline for the delinquent population.
    lateSplatter = vtkGaussianSplatter()
    lateSplatter.SetInputConnection(fd2ad.GetOutputPort())
    lateSplatter.SetSampleDimensions(150, 150, 150)
    lateSplatter.SetRadius(0.05)
    lateSplatter.SetScaleFactor(0.05)

    lateSurface = vtkMarchingContourFilter()
    lateSurface.SetInputConnection(lateSplatter.GetOutputPort())
    lateSurface.SetValue(0, 0.01)
    lateMapper = vtkPolyDataMapper()
    lateMapper.SetInputConnection(lateSurface.GetOutputPort())
    lateMapper.ScalarVisibilityOff()
    lateActor = vtkActor()
    lateActor.SetMapper(lateMapper)
    lateActor.GetProperty().SetColor(colors.GetColor3d('Tomato'))

    # Create the axes.
    popSplatter.Update()
    bounds = popSplatter.GetOutput().GetBounds()
    axes = vtkAxes()
    axes.SetOrigin(bounds[0], bounds[2], bounds[4])
    axes.SetScaleFactor(popSplatter.GetOutput().GetLength() / 5.0)
    axesTubes = vtkTubeFilter()
    axesTubes.SetInputConnection(axes.GetOutputPort())
    axesTubes.SetRadius(axes.GetScaleFactor() / 25.0)
    axesTubes.SetNumberOfSides(6)
    axesMapper = vtkPolyDataMapper()
    axesMapper.SetInputConnection(axesTubes.GetOutputPort())
    axesActor = vtkActor()
    axesActor.SetMapper(axesMapper)

    # Label the axes.
    XText = vtkVectorText()
    XText.SetText(xAxis)
    XTextMapper = vtkPolyDataMapper()
    XTextMapper.SetInputConnection(XText.GetOutputPort())

    XActor = vtkFollower()
    XActor.SetMapper(XTextMapper)
    XActor.SetScale(0.02, .02, .02)
    XActor.SetPosition(0.35, -0.05, -0.05)
    XActor.GetProperty().SetColor(0, 0, 0)

    YText = vtkVectorText()
    YText.SetText(yAxis)

    YTextMapper = vtkPolyDataMapper()
    YTextMapper.SetInputConnection(YText.GetOutputPort())
    YActor = vtkFollower()
    YActor.SetMapper(YTextMapper)
    YActor.SetScale(0.02, .02, .02)
    YActor.SetPosition(-0.05, 0.35, -0.05)
    YActor.GetProperty().SetColor(0, 0, 0)

    ZText = vtkVectorText()
    ZText.SetText(zAxis)
    ZTextMapper = vtkPolyDataMapper()
    ZTextMapper.SetInputConnection(ZText.GetOutputPort())
    ZActor = vtkFollower()
    ZActor.SetMapper(ZTextMapper)
    ZActor.SetScale(0.02, .02, .02)
    ZActor.SetPosition(-0.05, -0.05, 0.35)
    ZActor.GetProperty().SetColor(0, 0, 0)

    # Graphics stuff.
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('FinanceFieldData')

    # Add the actors to the renderer, set the background and size.
    renderer.AddActor(axesActor)
    renderer.AddActor(lateActor)
    renderer.AddActor(XActor)
    renderer.AddActor(YActor)
    renderer.AddActor(ZActor)
    renderer.AddActor(popActor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))
    renderWindow.SetSize(650, 480)

    camera = vtkCamera()
    camera.SetClippingRange(.274, 13.72)
    camera.SetFocalPoint(0.433816, 0.333131, 0.449)
    camera.SetPosition(-1.96987, 1.15145, 1.49053)
    camera.SetViewUp(0.378927, 0.911821, 0.158107)
    renderer.SetActiveCamera(camera)
    XActor.SetCamera(camera)
    YActor.SetCamera(camera)
    ZActor.SetCamera(camera)

    # Render and interact with the data.

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)
    renderWindow.Render()
    interactor.Start()


def get_program_parameters():
    import argparse
    description = 'Visualization of multidimensional financial data.'
    epilogue = '''
    This example is similar to /Python/Modelling/Finance.py, but here we read a .vtk file with vtkDataObjectReader.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('filename', help='financial.')

    args = parser.parse_args()
    return args.filename


if __name__ == '__main__':
    main()
