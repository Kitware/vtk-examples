#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkQuadric
from vtkmodules.vtkFiltersCore import vtkContourFilter
from vtkmodules.vtkFiltersModeling import vtkOutlineFilter
from vtkmodules.vtkImagingHybrid import vtkSampleFunction
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

    #
    # Create surfaces F(x,y,z) = constant
    #
    # Sample quadric function
    quadric = vtkQuadric()
    quadric.SetCoefficients(.5, 1, .2, 0, .1, 0, 0, .2, 0, 0)
    sample = vtkSampleFunction()
    sample.SetSampleDimensions(50, 50, 50)
    sample.SetImplicitFunction(quadric)

    contour = vtkContourFilter()
    contour.SetInputConnection(sample.GetOutputPort())
    contour.GenerateValues(5, 0, 1.2)

    contourMapper = vtkPolyDataMapper()
    contourMapper.SetInputConnection(contour.GetOutputPort())
    contourMapper.SetScalarRange(0, 1.2)

    contourActor = vtkActor()
    contourActor.SetMapper(contourMapper)

    # Create outline
    outline = vtkOutlineFilter()
    outline.SetInputConnection(sample.GetOutputPort())

    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())

    outlineActor = vtkActor()
    outlineActor.SetMapper(outlineMapper)
    outlineActor.GetProperty().SetColor(colors.GetColor3d("Brown"))
    outlineActor.GetProperty().SetLineWidth(3.0)

    #
    # Rendering stuff
    #
    aren.SetBackground(colors.GetColor3d("SlateGray"))
    aren.AddActor(contourActor)
    aren.AddActor(outlineActor)

    aren.ResetCamera()
    aren.GetActiveCamera().Azimuth(30)
    aren.GetActiveCamera().Elevation(30)

    renWin.SetSize(640, 512)
    renWin.SetWindowName('ContourQuadric')
    renWin.Render()

    # interact with data
    iren.Start()


if __name__ == '__main__':
    main()
