#!/usr/bin/env python

import math

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingContextOpenGL2
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkChartsCore import (
    vtkChart,
    vtkChartXY,
    vtkPlotPoints
)
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkFloatArray
from vtkmodules.vtkCommonDataModel import vtkTable
from vtkmodules.vtkViewsContext2D import vtkContextView


def main():
    colors = vtkNamedColors()

    view = vtkContextView()
    view.GetRenderer().SetBackground(colors.GetColor3d('SlateGray'))
    view.GetRenderWindow().SetSize(400, 300)

    chart = vtkChartXY()
    view.GetScene().AddItem(chart)
    chart.SetShowLegend(True)

    table = vtkTable()

    arrX = vtkFloatArray()
    arrX.SetName('X Axis')

    arrC = vtkFloatArray()
    arrC.SetName('Cosine')

    arrS = vtkFloatArray()
    arrS.SetName('Sine')

    arrT = vtkFloatArray()
    arrT.SetName('Sine-Cosine')

    table.AddColumn(arrC)
    table.AddColumn(arrS)
    table.AddColumn(arrX)
    table.AddColumn(arrT)

    numPoints = 40

    inc = 7.5 / (numPoints - 1)
    table.SetNumberOfRows(numPoints)
    for i in range(numPoints):
        table.SetValue(i, 0, i * inc)
        table.SetValue(i, 1, math.cos(i * inc))
        table.SetValue(i, 2, math.sin(i * inc))
        table.SetValue(i, 3, math.sin(i * inc) - math.cos(i * inc))

    points = chart.AddPlot(vtkChart.POINTS)
    points.SetInputData(table, 0, 1)
    points.SetColor(0, 0, 0, 255)
    points.SetWidth(1.0)
    points.SetMarkerStyle(vtkPlotPoints.CROSS)

    points = chart.AddPlot(vtkChart.POINTS)
    points.SetInputData(table, 0, 2)
    points.SetColor(0, 0, 0, 255)
    points.SetWidth(1.0)
    points.SetMarkerStyle(vtkPlotPoints.PLUS)

    points = chart.AddPlot(vtkChart.POINTS)
    points.SetInputData(table, 0, 3)
    points.SetColor(0, 0, 255, 255)
    points.SetWidth(1.0)
    points.SetMarkerStyle(vtkPlotPoints.CIRCLE)

    view.GetRenderWindow().SetMultiSamples(0)
    view.GetRenderWindow().SetWindowName('ScatterPlot')
    view.GetInteractor().Initialize()
    view.GetInteractor().Start()


if __name__ == '__main__':
    main()
