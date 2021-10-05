#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import (
    vtkImplicitBoolean,
    vtkQuadric,
    vtkSphere
)
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersExtraction import vtkExtractGeometry
from vtkmodules.vtkFiltersGeneral import vtkShrinkFilter
from vtkmodules.vtkFiltersModeling import vtkOutlineFilter
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

    ren1 = vtkRenderer()

    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren1)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    quadric = vtkQuadric()
    quadric.SetCoefficients(0.5, 1, 0.2, 0, 0.1, 0, 0, 0.2, 0, 0)

    sample = vtkSampleFunction()
    sample.SetSampleDimensions(50, 50, 50)
    sample.SetImplicitFunction(quadric)
    sample.ComputeNormalsOff()

    trans = vtkTransform()
    trans.Scale(1, 0.5, 0.333)

    sphere = vtkSphere()
    sphere.SetRadius(0.25)
    sphere.SetTransform(trans)

    trans2 = vtkTransform()
    trans2.Scale(0.25, 0.5, 1.0)

    sphere2 = vtkSphere()
    sphere2.SetRadius(0.25)
    sphere2.SetTransform(trans2)

    booleanUnion = vtkImplicitBoolean()
    booleanUnion.AddFunction(sphere)
    booleanUnion.AddFunction(sphere2)
    booleanUnion.SetOperationType(0)  # boolean Union

    extract = vtkExtractGeometry()
    extract.SetInputConnection(sample.GetOutputPort())
    extract.SetImplicitFunction(booleanUnion)

    shrink = vtkShrinkFilter()
    shrink.SetInputConnection(extract.GetOutputPort())
    shrink.SetShrinkFactor(0.5)

    dataMapper = vtkDataSetMapper()
    dataMapper.SetInputConnection(shrink.GetOutputPort())
    dataActor = vtkActor()
    dataActor.SetMapper(dataMapper)

    # outline
    outline = vtkOutlineFilter()
    outline.SetInputConnection(sample.GetOutputPort())

    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())

    outlineActor = vtkActor()
    outlineActor.SetMapper(outlineMapper)
    outlineActor.GetProperty().SetColor(0, 0, 0)

    # Add the actors to the renderer, set the background and size
    #
    ren1.AddActor(outlineActor)
    ren1.AddActor(dataActor)
    ren1.SetBackground(colors.GetColor3d("SlateGray"))

    renWin.SetSize(640, 480)
    renWin.SetWindowName('ExtractData')

    renWin.Render()
    ren1.GetActiveCamera().Azimuth(30)
    ren1.GetActiveCamera().Elevation(30)

    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
