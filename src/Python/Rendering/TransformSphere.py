#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkLookupTable
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersCore import vtkElevationFilter
from vtkmodules.vtkFiltersGeneral import vtkTransformFilter
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
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

    sphere = vtkSphereSource()
    sphere.SetThetaResolution(12)
    sphere.SetPhiResolution(12)

    aTransform = vtkTransform()
    aTransform.Scale(1, 1.5, 2)

    transFilter = vtkTransformFilter()
    transFilter.SetInputConnection(sphere.GetOutputPort())
    transFilter.SetTransform(aTransform)

    colorIt = vtkElevationFilter()
    colorIt.SetInputConnection(transFilter.GetOutputPort())
    colorIt.SetLowPoint(0, 0, -1)
    colorIt.SetHighPoint(0, 0, 1)

    lut = vtkLookupTable()
    lut.SetHueRange(0.667, 0)
    lut.SetSaturationRange(1, 1)
    lut.SetValueRange(1, 1)

    mapper = vtkDataSetMapper()
    mapper.SetLookupTable(lut)
    mapper.SetInputConnection(colorIt.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d("SlateGray"))
    renderer.ResetCamera()
    renderer.GetActiveCamera().Elevation(60.0)
    renderer.GetActiveCamera().Azimuth(30.0)
    renderer.ResetCameraClippingRange()

    renWin.SetSize(640, 480)
    renWin.SetWindowName('TransformSphere')

    renWin.Render()

    # Interact with the data.
    iren.Start()


if __name__ == '__main__':
    main()
