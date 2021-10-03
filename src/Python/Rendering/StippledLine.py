#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    VTK_UNSIGNED_CHAR,
    vtkDoubleArray
)
from vtkmodules.vtkCommonDataModel import vtkImageData
from vtkmodules.vtkFiltersSources import vtkLineSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTexture
)


def main():
    colors = vtkNamedColors()

    lines = vtkLineSource()
    # Create two points, P0 and P1
    p0 = [1.0, 0.0, 0.0]
    p1 = [5.0, 0.0, 0.0]

    lines.SetResolution(11)
    lines.SetPoint1(p0)
    lines.SetPoint2(p1)
    lines.Update()
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(lines.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetLineWidth(5)
    actor.GetProperty().SetColor(colors.GetColor3d("Banana"))

    StippledLine(actor, 0xA1A1, 2)

    ren1 = vtkRenderer()
    ren1.SetBackground(colors.GetColor3d("SlateGray"))
    renWin = vtkRenderWindow()
    renWin.SetSize(640, 480)
    renWin.SetWindowName('StippledLine')

    renWin.AddRenderer(ren1)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)
    ren1.AddActor(actor)
    renWin.Render()
    iren.Start()


def StippledLine(actor, lineStipplePattern, lineStippleRepeat):
    tcoords = vtkDoubleArray()
    image = vtkImageData()
    texture = vtkTexture()

    # Create texture
    dimension = 16 * lineStippleRepeat

    image.SetDimensions(dimension, 1, 1)
    image.AllocateScalars(VTK_UNSIGNED_CHAR, 4)
    image.SetExtent(0, dimension - 1, 0, 0, 0, 0)
    on = 255
    off = 0
    i_dim = 0
    while i_dim < dimension:
        for i in range(0, 16):
            mask = (1 << i)
            bit = (lineStipplePattern & mask) >> i
            value = bit
            if value == 0:
                for j in range(0, lineStippleRepeat):
                    image.SetScalarComponentFromFloat(i_dim, 0, 0, 0, on)
                    image.SetScalarComponentFromFloat(i_dim, 0, 0, 1, on)
                    image.SetScalarComponentFromFloat(i_dim, 0, 0, 2, on)
                    image.SetScalarComponentFromFloat(i_dim, 0, 0, 3, off)
                    i_dim += 1
            else:
                for j in range(0, lineStippleRepeat):
                    image.SetScalarComponentFromFloat(i_dim, 0, 0, 0, on)
                    image.SetScalarComponentFromFloat(i_dim, 0, 0, 1, on)
                    image.SetScalarComponentFromFloat(i_dim, 0, 0, 2, on)
                    image.SetScalarComponentFromFloat(i_dim, 0, 0, 3, on)
                    i_dim += 1
    polyData = actor.GetMapper().GetInput()
    # Create texture coordinates
    tcoords.SetNumberOfComponents(1)
    tcoords.SetNumberOfTuples(polyData.GetNumberOfPoints())
    for i in range(0, polyData.GetNumberOfPoints()):
        value = i * 0.5
        tcoords.SetTypedTuple(i, [value])
    polyData.GetPointData().SetTCoords(tcoords)
    texture.SetInputData(image)
    texture.InterpolateOff()
    texture.RepeatOn()

    actor.SetTexture(texture)


if __name__ == '__main__':
    main()
