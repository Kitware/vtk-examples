# !/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonComputationalGeometry import vtkParametricSuperToroid
from vtkmodules.vtkCommonCore import (
    vtkCommand,
    vtkMath
)
from vtkmodules.vtkFiltersSources import vtkParametricFunctionSource
from vtkmodules.vtkInteractionWidgets import (
    vtkSliderRepresentation2D,
    vtkSliderWidget
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkProperty,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    colors.SetColor('BkgColor', [26, 51, 102, 255])

    surface = vtkParametricSuperToroid()
    source = vtkParametricFunctionSource()

    renderer = vtkRenderer()
    mapper = vtkPolyDataMapper()
    actor = vtkActor()

    backProperty = vtkProperty()
    backProperty.SetColor(colors.GetColor3d('Tomato'))

    # Create a parametric function source, renderer, mapper, and actor
    source.SetParametricFunction(surface)

    mapper.SetInputConnection(source.GetOutputPort())

    actor.SetMapper(mapper)
    actor.SetBackfaceProperty(backProperty)
    actor.GetProperty().SetDiffuseColor(colors.GetColor3d('Banana'))
    actor.GetProperty().SetSpecular(.5)
    actor.GetProperty().SetSpecularPower(20)

    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName('ParametricSuperToroidDemo')
    renderWindow.AddRenderer(renderer)
    renderWindow.SetSize(640, 480)
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('BkgColor'))
    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(30)
    renderer.GetActiveCamera().Elevation(-30)
    renderer.GetActiveCamera().Zoom(0.9)
    renderer.ResetCameraClippingRange()

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    # Setup a slider widget for each varying parameter
    tubeWidth = 0.008
    sliderLength = 0.008
    titleHeight = 0.04
    labelHeight = 0.04

    sliderRepN1 = vtkSliderRepresentation2D()

    sliderRepN1.SetMinimumValue(0.0)
    sliderRepN1.SetMaximumValue(4.0)
    sliderRepN1.SetValue(1.0)
    sliderRepN1.SetTitleText('Z squareness')

    sliderRepN1.GetPoint1Coordinate().SetCoordinateSystemToNormalizedDisplay()
    sliderRepN1.GetPoint1Coordinate().SetValue(.1, .1)
    sliderRepN1.GetPoint2Coordinate().SetCoordinateSystemToNormalizedDisplay()
    sliderRepN1.GetPoint2Coordinate().SetValue(.9, .1)

    sliderRepN1.SetTubeWidth(tubeWidth)
    sliderRepN1.SetSliderLength(sliderLength)
    sliderRepN1.SetTitleHeight(titleHeight)
    sliderRepN1.SetLabelHeight(labelHeight)

    sliderWidgetN1 = vtkSliderWidget()
    sliderWidgetN1.SetInteractor(interactor)
    sliderWidgetN1.SetRepresentation(sliderRepN1)
    sliderWidgetN1.SetAnimationModeToAnimate()
    sliderWidgetN1.EnabledOn()

    sliderWidgetN1.AddObserver(vtkCommand.InteractionEvent, SliderCallbackN1(surface))

    sliderRepN2 = vtkSliderRepresentation2D()

    sliderRepN2.SetMinimumValue(0.0001)
    sliderRepN2.SetMaximumValue(4.0)
    sliderRepN2.SetValue(1.0)
    sliderRepN2.SetTitleText('XY squareness')

    sliderRepN2.GetPoint1Coordinate().SetCoordinateSystemToNormalizedDisplay()
    sliderRepN2.GetPoint1Coordinate().SetValue(.1, .9)
    sliderRepN2.GetPoint2Coordinate().SetCoordinateSystemToNormalizedDisplay()
    sliderRepN2.GetPoint2Coordinate().SetValue(.9, .9)

    sliderRepN2.SetTubeWidth(tubeWidth)
    sliderRepN2.SetSliderLength(sliderLength)
    sliderRepN2.SetTitleHeight(titleHeight)
    sliderRepN2.SetLabelHeight(labelHeight)

    sliderWidgetN2 = vtkSliderWidget()
    sliderWidgetN2.SetInteractor(interactor)
    sliderWidgetN2.SetRepresentation(sliderRepN2)
    sliderWidgetN2.SetAnimationModeToAnimate()
    sliderWidgetN2.EnabledOn()

    sliderWidgetN2.AddObserver(vtkCommand.InteractionEvent, SliderCallbackN2(surface))

    sliderRepMinimumV = vtkSliderRepresentation2D()

    sliderRepN1.SetMinimumValue(.0001)
    sliderRepMinimumV.SetMaximumValue(.9999 * vtkMath.Pi())
    sliderRepMinimumV.SetValue(.0001)
    sliderRepMinimumV.SetTitleText('V min')

    sliderRepMinimumV.GetPoint1Coordinate().SetCoordinateSystemToNormalizedDisplay()
    sliderRepMinimumV.GetPoint1Coordinate().SetValue(.1, .1)
    sliderRepMinimumV.GetPoint2Coordinate().SetCoordinateSystemToNormalizedDisplay()
    sliderRepMinimumV.GetPoint2Coordinate().SetValue(.1, .9)

    sliderRepMinimumV.SetTubeWidth(tubeWidth)
    sliderRepMinimumV.SetSliderLength(sliderLength)
    sliderRepMinimumV.SetTitleHeight(titleHeight)
    sliderRepMinimumV.SetLabelHeight(labelHeight)

    surface.SetN1(1.0)
    surface.SetN2(1.0)

    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(30)
    renderer.GetActiveCamera().Elevation(-30)
    renderer.GetActiveCamera().Zoom(0.9)
    renderer.ResetCameraClippingRange()
    renderWindow.Render()

    interactor.Initialize()

    interactor.Start()


# These callbacks do the actual work.
# Callbacks for the interactions

class SliderCallbackN1():
    def __init__(self, superToroid):
        self.superToroid = superToroid

    def __call__(self, caller, ev):
        sliderWidget = caller
        value = sliderWidget.GetRepresentation().GetValue()
        self.superToroid.SetN1(value)


class SliderCallbackN2():
    def __init__(self, superToroid):
        self.superToroid = superToroid

    def __call__(self, caller, ev):
        sliderWidget = caller
        value = sliderWidget.GetRepresentation().GetValue()
        self.superToroid.SetN2(value)


if __name__ == '__main__':
    main()
