#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkQuadric
from vtkmodules.vtkFiltersCore import (
    vtkAppendFilter,
    vtkContourFilter
)
from vtkmodules.vtkFiltersModeling import vtkOutlineFilter
from vtkmodules.vtkImagingCore import vtkExtractVOI
from vtkmodules.vtkImagingHybrid import vtkSampleFunction
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    renderer = vtkRenderer()

    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)
    renderWindow.SetSize(640, 480)

    #
    # Create surface of implicit function.
    #

    # Sample quadric function.
    quadric = vtkQuadric()
    quadric.SetCoefficients(1, 2, 3, 0, 1, 0, 0, 0, 0, 0)

    sample = vtkSampleFunction()
    sample.SetSampleDimensions(25, 25, 25)
    sample.SetImplicitFunction(quadric)

    isoActor = vtkActor()
    CreateIsosurface(sample, isoActor)
    outlineIsoActor = vtkActor()
    CreateOutline(sample, outlineIsoActor)

    planesActor = vtkActor()
    CreatePlanes(sample, planesActor, 3)
    outlinePlanesActor = vtkActor()
    CreateOutline(sample, outlinePlanesActor)
    planesActor.AddPosition(isoActor.GetBounds()[0] * 2.0, 0, 0)
    outlinePlanesActor.AddPosition(isoActor.GetBounds()[0] * 2.0, 0, 0)

    contourActor = vtkActor()
    CreateContours(sample, contourActor, 3, 15)
    outlineContourActor = vtkActor()
    CreateOutline(sample, outlineContourActor)
    contourActor.AddPosition(isoActor.GetBounds()[0] * 4.0, 0, 0)
    outlineContourActor.AddPosition(isoActor.GetBounds()[0] * 4.0, 0, 0)

    renderer.AddActor(planesActor)
    renderer.AddActor(outlinePlanesActor)
    renderer.AddActor(contourActor)
    renderer.AddActor(outlineContourActor)
    renderer.AddActor(isoActor)
    renderer.AddActor(outlineIsoActor)

    renderer.TwoSidedLightingOn()

    renderer.SetBackground(colors.GetColor3d("SlateGray"))

    # Try to set camera to match figure on book
    renderer.GetActiveCamera().SetPosition(0, -1, 0)
    renderer.GetActiveCamera().SetFocalPoint(0, 0, 0)
    renderer.GetActiveCamera().SetViewUp(0, 0, -1)
    renderer.ResetCamera()
    renderer.GetActiveCamera().Elevation(20)
    renderer.GetActiveCamera().Azimuth(10)
    renderer.GetActiveCamera().Dolly(1.2)
    renderer.ResetCameraClippingRange()

    renderWindow.SetSize(640, 480)
    renderWindow.SetWindowName('QuadricVisualization');

    renderWindow.Render()

    # interact with data
    interactor.Start()


def CreateIsosurface(func, actor, numberOfContours=5):
    # Generate implicit surface.
    contour = vtkContourFilter()
    contour.SetInputConnection(func.GetOutputPort())
    ranges = [1.0, 6.0]
    contour.GenerateValues(numberOfContours, ranges)

    # Map contour
    contourMapper = vtkPolyDataMapper()
    contourMapper.SetInputConnection(contour.GetOutputPort())
    contourMapper.SetScalarRange(0, 7)

    actor.SetMapper(contourMapper)
    return


def CreatePlanes(func, actor, numberOfPlanes):
    #
    # Extract planes from implicit function.
    #

    append = vtkAppendFilter()

    dims = func.GetSampleDimensions()
    sliceIncr = (dims[2] - 1) // (numberOfPlanes + 1)
    sliceNum = -4
    for i in range(0, numberOfPlanes):
        extract = vtkExtractVOI()
        extract.SetInputConnection(func.GetOutputPort())
        extract.SetVOI(0, dims[0] - 1,
                       0, dims[1] - 1,
                       sliceNum + sliceIncr, sliceNum + sliceIncr)
        append.AddInputConnection(extract.GetOutputPort())
        sliceNum += sliceIncr
    append.Update()

    # Map planes
    planesMapper = vtkDataSetMapper()
    planesMapper.SetInputConnection(append.GetOutputPort())
    planesMapper.SetScalarRange(0, 7)

    actor.SetMapper(planesMapper)
    actor.GetProperty().SetAmbient(1.)
    return


def CreateContours(func, actor, numberOfPlanes, numberOfContours):
    #
    # Extract planes from implicit function
    #

    append = vtkAppendFilter()

    dims = func.GetSampleDimensions()
    sliceIncr = (dims[2] - 1) // (numberOfPlanes + 1)

    sliceNum = -4
    for i in range(0, numberOfPlanes):
        extract = vtkExtractVOI()
        extract.SetInputConnection(func.GetOutputPort())
        extract.SetVOI(0, dims[0] - 1,
                       0, dims[1] - 1,
                       sliceNum + sliceIncr, sliceNum + sliceIncr)
        ranges = [1.0, 6.0]
        contour = vtkContourFilter()
        contour.SetInputConnection(extract.GetOutputPort())
        contour.GenerateValues(numberOfContours, ranges)
        append.AddInputConnection(contour.GetOutputPort())
        sliceNum += sliceIncr
    append.Update()

    # Map planes
    planesMapper = vtkDataSetMapper()
    planesMapper.SetInputConnection(append.GetOutputPort())
    planesMapper.SetScalarRange(0, 7)

    actor.SetMapper(planesMapper)
    actor.GetProperty().SetAmbient(1.)
    return


def CreateOutline(source, actor):
    outline = vtkOutlineFilter()
    outline.SetInputConnection(source.GetOutputPort())
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(outline.GetOutputPort())
    actor.SetMapper(mapper)
    return


if __name__ == '__main__':
    main()
