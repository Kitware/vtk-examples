#!/usr/bin/env python
# -*- coding: utf-8 -*-

import math

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkMath,
    vtkMinimalStandardRandomSequence,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData,
    vtkPolyLine
)
from vtkmodules.vtkCommonMath import vtkMatrix4x4
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersCore import vtkTubeFilter
from vtkmodules.vtkFiltersGeneral import vtkTransformPolyDataFilter
from vtkmodules.vtkFiltersModeling import vtkLinearExtrusionFilter
from vtkmodules.vtkFiltersSources import vtkArrowSource
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    nx, ny, nz = get_program_parameters()
    colors = vtkNamedColors()

    angle = 0
    r1 = 50
    r2 = 30
    centerX = 10.0
    centerY = 5.0

    points = vtkPoints()
    idx = 0
    while angle <= 2.0 * vtkMath.Pi() + (vtkMath.Pi() / 60.0):
        points.InsertNextPoint(r1 * math.cos(angle) + centerX,
                               r2 * math.sin(angle) + centerY,
                               0.0)
        angle = angle + (vtkMath.Pi() / 60.0)
        idx += 1

    line = vtkPolyLine()
    line.GetPointIds().SetNumberOfIds(idx)
    for i in range(0, idx):
        line.GetPointIds().SetId(i, i)

    lines = vtkCellArray()
    lines.InsertNextCell(line)

    polyData = vtkPolyData()
    polyData.SetPoints(points)
    polyData.SetLines(lines)

    extrude = vtkLinearExtrusionFilter()
    extrude.SetInputData(polyData)
    extrude.SetExtrusionTypeToNormalExtrusion()
    extrude.SetVector(nx, ny, nz)
    extrude.Update()

    # Create an oriented arrow
    startPoint = [0.0] * 3
    endPoint = [0.0] * 3
    startPoint[0] = centerX
    startPoint[1] = centerY
    startPoint[2] = 0.0
    for i in range(0, 3):
        endPoint[i] = startPoint[i] + extrude.GetVector()[i]

    # Compute a basis
    normalizedX = [0.0] * 3
    normalizedY = [0.0] * 3
    normalizedZ = [0.0] * 3

    # The X axis is a vector from start to end
    vtkMath.Subtract(endPoint, startPoint, normalizedX)
    length = vtkMath.Norm(normalizedX)
    vtkMath.Normalize(normalizedX)

    rng = vtkMinimalStandardRandomSequence()
    rng.SetSeed(8775070)
    max_r = 10.0

    # The Z axis is an arbitrary vector cross X
    arbitrary = [0.0] * 3
    for i in range(0, 3):
        arbitrary[i] = rng.GetRangeValue(-max_r, max_r)
        rng.Next()
    vtkMath.Cross(normalizedX, arbitrary, normalizedZ)
    vtkMath.Normalize(normalizedZ)

    # The Y axis is Z cross X
    vtkMath.Cross(normalizedZ, normalizedX, normalizedY)
    matrix = vtkMatrix4x4()

    # Create the direction cosine matrix
    matrix.Identity()
    for i in range(0, 3):
        matrix.SetElement(i, 0, normalizedX[i])
        matrix.SetElement(i, 1, normalizedY[i])
        matrix.SetElement(i, 2, normalizedZ[i])

    # Apply the transforms
    transform = vtkTransform()
    transform.Translate(startPoint)
    transform.Concatenate(matrix)
    transform.Scale(length, length, length)

    arrowSource = vtkArrowSource()
    arrowSource.SetTipResolution(31)
    arrowSource.SetShaftResolution(21)

    # Transform the polydata
    transformPD = vtkTransformPolyDataFilter()
    transformPD.SetTransform(transform)
    transformPD.SetInputConnection(arrowSource.GetOutputPort())

    # Create a mapper and actor for the arrow
    arrowMapper = vtkPolyDataMapper()
    arrowMapper.SetInputConnection(transformPD.GetOutputPort())

    arrowActor = vtkActor()
    arrowActor.SetMapper(arrowMapper)
    arrowActor.GetProperty().SetColor(colors.GetColor3d("Tomato"))

    tubes = vtkTubeFilter()
    tubes.SetInputData(polyData)
    tubes.SetRadius(2.0)
    tubes.SetNumberOfSides(21)

    lineMapper = vtkPolyDataMapper()
    lineMapper.SetInputConnection(tubes.GetOutputPort())

    lineActor = vtkActor()
    lineActor.SetMapper(lineMapper)
    lineActor.GetProperty().SetColor(colors.GetColor3d("Peacock"))

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(extrude.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d("Banana"))
    actor.GetProperty().SetOpacity(.7)

    ren = vtkRenderer()
    ren.SetBackground(colors.GetColor3d("SlateGray"))
    ren.AddActor(actor)
    ren.AddActor(lineActor)
    ren.AddActor(arrowActor)

    renWin = vtkRenderWindow()
    renWin.SetWindowName("Elliptical Cylinder Demo")
    renWin.AddRenderer(ren)
    renWin.SetSize(600, 600)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    style = vtkInteractorStyleTrackballCamera()
    iren.SetInteractorStyle(style)

    camera = vtkCamera()
    camera.SetPosition(0, 1, 0)
    camera.SetFocalPoint(0, 0, 0)
    camera.SetViewUp(0, 0, 1)
    camera.Azimuth(30)
    camera.Elevation(30)

    ren.SetActiveCamera(camera)
    ren.ResetCamera()
    ren.ResetCameraClippingRange()

    renWin.Render()
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'Elliptical Cylinder Demo.'
    epilogue = '''
The example shows the vtkPolyLine that forms the base of the elliptical cylinder
 and an oriented arrow that represents the vector that vtkLinearExtrusionFilter
 uses to create the cylinder.
The example takes an optional triple that defines the vector for the filter.
The length of the vector is the height of the cylinder.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-nx', nargs='?', const=0.0, default=0.0, type=float)
    parser.add_argument('-ny', nargs='?', const=0.0, default=0.0, type=float)
    parser.add_argument('-nz', nargs='?', const=100.0, default=100.0, type=float)
    args = parser.parse_args()
    return args.nx, args.ny, args.nz


if __name__ == '__main__':
    main()
