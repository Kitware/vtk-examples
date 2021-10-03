#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkDoubleArray
from vtkmodules.vtkCommonDataModel import vtkStructuredPoints
from vtkmodules.vtkFiltersCore import vtkContourFilter
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    renderer = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(renderer)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    vol = vtkStructuredPoints()
    vol.SetDimensions(26, 26, 26)
    vol.SetOrigin(-0.5, -0.5, -0.5)
    sp = 1.0 / 25.0
    vol.SetSpacing(sp, sp, sp)

    scalars = vtkDoubleArray()
    scalars.SetNumberOfComponents(1)
    scalars.SetNumberOfTuples(26 * 26 * 26)
    for k in range(0, 26):
        z = -0.5 + k * sp
        kOffset = k * 26 * 26
        for j in range(0, 26):
            y = -0.5 + j * sp
            jOffset = j * 26
            for i in range(0, 26):
                x = -0.5 + i * sp
                s = x * x + y * y + z * z - (0.4 * 0.4)
                offset = i + jOffset + kOffset
                scalars.InsertTuple1(offset, s)
    vol.GetPointData().SetScalars(scalars)

    contour = vtkContourFilter()
    contour.SetInputData(vol)
    contour.SetValue(0, 0.0)

    volMapper = vtkPolyDataMapper()
    volMapper.SetInputConnection(contour.GetOutputPort())
    volMapper.ScalarVisibilityOff()
    volActor = vtkActor()
    volActor.SetMapper(volMapper)
    volActor.GetProperty().EdgeVisibilityOn()
    volActor.GetProperty().SetColor(colors.GetColor3d('Salmon'))
    renderer.AddActor(volActor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))
    renWin.SetSize(512, 512)
    renWin.SetWindowName('Vol')

    # Interact with the data.
    renWin.Render()

    iren.Start()


if __name__ == '__main__':
    main()
