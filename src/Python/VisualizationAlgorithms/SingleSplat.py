#!/usr/bin/env python


# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkDoubleArray,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData
)
from vtkmodules.vtkFiltersCore import vtkContourFilter
from vtkmodules.vtkFiltersModeling import vtkOutlineFilter
from vtkmodules.vtkFiltersSources import vtkConeSource
from vtkmodules.vtkImagingHybrid import vtkGaussianSplatter
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    aren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(aren)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Create single splat point
    pts = vtkPoints()
    verts = vtkCellArray()
    norms = vtkDoubleArray()
    scalars = vtkDoubleArray()

    x = [0.0] * 3
    pts.InsertNextPoint(x)
    norms.SetNumberOfTuples(1)
    norms.SetNumberOfComponents(3)
    n = [0] * 3
    n[0] = 0.707
    n[1] = 0.707
    n[2] = 0.0
    norms.InsertTuple(0, n)
    scalars.SetNumberOfTuples(1)
    scalars.SetNumberOfComponents(1)
    scalars.InsertTuple1(0, 1.0)

    verts.InsertNextCell(1)
    verts.InsertCellPoint(0)

    pData = vtkPolyData()
    pData.SetPoints(pts)
    pData.SetVerts(verts)
    pData.GetPointData().SetNormals(norms)
    pData.GetPointData().SetScalars(scalars)

    # Splat point and generate isosurface.
    splat = vtkGaussianSplatter()
    splat.SetInputData(pData)
    splat.SetModelBounds(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0)
    splat.SetSampleDimensions(75, 75, 75)
    splat.SetRadius(0.5)
    splat.SetEccentricity(5.0)
    splat.SetExponentFactor(-3.25)
    contour = vtkContourFilter()
    contour.SetInputConnection(splat.GetOutputPort())
    contour.SetValue(0, 0.9)
    splatMapper = vtkPolyDataMapper()
    splatMapper.SetInputConnection(contour.GetOutputPort())
    splatActor = vtkActor()
    splatActor.SetMapper(splatMapper)

    # Create outline.
    outline = vtkOutlineFilter()
    outline.SetInputConnection(splat.GetOutputPort())
    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())
    outlineActor = vtkActor()
    outlineActor.SetMapper(outlineMapper)
    outlineActor.GetProperty().SetColor(colors.GetColor3d('Brown'))

    # Create cone to indicate direction.
    cone = vtkConeSource()
    cone.SetResolution(24)
    coneMapper = vtkPolyDataMapper()
    coneMapper.SetInputConnection(cone.GetOutputPort())
    coneActor = vtkActor()
    coneActor.SetMapper(coneMapper)
    coneActor.SetScale(0.75, 0.75, 0.75)
    coneActor.RotateZ(45.0)
    coneActor.AddPosition(0.50, 0.50, 0.0)
    coneActor.GetProperty().SetColor(colors.GetColor3d('DeepPink'))
    #
    # Rendering stuff.
    #
    aren.SetBackground(colors.GetColor3d('Beige'))
    aren.AddActor(splatActor)
    aren.AddActor(outlineActor)
    aren.AddActor(coneActor)

    renWin.SetSize(640, 640)
    renWin.SetWindowName('SingleSplat')
    renWin.Render()

    # Interact with the data.
    iren.Start()


if __name__ == '__main__':
    main()
