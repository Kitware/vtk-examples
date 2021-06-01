import math

import vtk


def main():
    colors = vtk.vtkNamedColors()

    renwin = vtk.vtkRenderWindow()
    renwin.SetSize(640, 480)
    renwin.SetWindowName('MultipleGraphs')
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(renwin)

    # Setup the viewports
    grid_dimensions_x = 2
    grid_dimensions_y = 1
    rendere_sz_x = 320
    renderer_sz_y = 240
    renwin.SetSize(rendere_sz_x * grid_dimensions_x, renderer_sz_y * grid_dimensions_y)

    viewports = list()
    for row in range(0, grid_dimensions_y):
        for col in range(0, grid_dimensions_x):
            # index = row * grid_dimensions_x + col

            # (xmin, ymin, xmax, ymax)
            viewports.append([float(col) / grid_dimensions_x,
                              float(grid_dimensions_y - (row + 1)) / grid_dimensions_y,
                              float(col + 1) / grid_dimensions_x,
                              float(grid_dimensions_y - row) / grid_dimensions_y])

    #
    left_renderer = vtk.vtkRenderer()
    left_renderer.SetBackground(colors.GetColor3d('AliceBlue'))
    left_renderer.SetViewport(viewports[0])
    renwin.AddRenderer(left_renderer)

    right_renderer = vtk.vtkRenderer()
    right_renderer.SetBackground(colors.GetColor3d('Lavender'))
    right_renderer.SetViewport(viewports[1])
    renwin.AddRenderer(right_renderer)

    left_chart_scene = vtk.vtkContextScene()
    left_chart_actor = vtk.vtkContextActor()
    left_chart_actor.SetScene(left_chart_scene)
    left_renderer.AddActor(left_chart_actor)
    left_chart_scene.SetRenderer(left_renderer)

    left_chart = vtk.vtkChartXY()
    xAxis = left_chart.GetAxis(vtk.vtkAxis.BOTTOM)
    xAxis.GetGridPen().SetColor(colors.GetColor4ub("LightGrey"))
    xAxis.SetTitle('x')
    yAxis = left_chart.GetAxis(vtk.vtkAxis.LEFT)
    yAxis.GetGridPen().SetColor(colors.GetColor4ub("LightGrey"))
    yAxis.SetTitle('cos(x)')
    left_chart.GetBackgroundBrush().SetColorF(*colors.GetColor4d('MistyRose'))
    left_chart.GetBackgroundBrush().SetOpacityF(0.4)
    left_chart.SetTitle('Cosine')
    left_chart_scene.AddItem(left_chart)

    right_chart_scene = vtk.vtkContextScene()
    right_chart_actor = vtk.vtkContextActor()
    right_chart_actor.SetScene(right_chart_scene)
    right_renderer.AddActor(right_chart_actor)
    right_chart = vtk.vtkChartXY()
    xAxis = right_chart.GetAxis(vtk.vtkAxis.BOTTOM)
    xAxis.GetGridPen().SetColor(colors.GetColor4ub("LightCyan"))
    xAxis.SetTitle('x')
    yAxis = right_chart.GetAxis(vtk.vtkAxis.LEFT)
    yAxis.GetGridPen().SetColor(colors.GetColor4ub("LightCyan"))
    yAxis.SetTitle('sin(x)')
    right_chart.GetBackgroundBrush().SetColorF(*colors.GetColor4d('Thistle'))
    right_chart.GetBackgroundBrush().SetOpacityF(0.4)
    right_chart.SetTitle('Sine')
    right_chart_scene.AddItem(right_chart)

    # Create the data
    table = vtk.vtkTable()

    arrX = vtk.vtkFloatArray()
    arrX.SetName('X Axis')

    arrC = vtk.vtkFloatArray()
    arrC.SetName('Cosine')

    arrS = vtk.vtkFloatArray()
    arrS.SetName('Sine')

    table.AddColumn(arrC)
    table.AddColumn(arrS)
    table.AddColumn(arrX)

    numPoints = 40

    inc = 7.5 / (numPoints - 1)
    table.SetNumberOfRows(numPoints)
    for i in range(numPoints):
        table.SetValue(i, 0, i * inc)
        table.SetValue(i, 1, math.cos(i * inc))
        table.SetValue(i, 2, math.sin(i * inc))

    points = left_chart.AddPlot(vtk.vtkChart.POINTS)
    points.SetInputData(table, 0, 1)
    points.SetColor(0, 0, 0, 255)
    points.SetWidth(1.0)
    points.SetMarkerStyle(vtk.vtkPlotPoints.CROSS)

    points = right_chart.AddPlot(vtk.vtkChart.POINTS)
    points.SetInputData(table, 0, 2)
    points.SetColor(0, 0, 0, 255)
    points.SetWidth(1.0)
    points.SetMarkerStyle(vtk.vtkPlotPoints.PLUS)

    renwin.SetMultiSamples(0)
    renwin.Render()
    # print(f"Left point1:  {left_chart.GetPoint1()}") # Equals chart_left.GetAxis(1).GetPoint1() instead of (10,10)?
    # print(f"Left point1:  {left_chart.GetAxis(1).GetPoint1()}")
    # print(f"Left point2:  {left_chart.GetPoint2()}")  # Equals chart_left.GetAxis(1).GetPoint1() instead of (10,10)?
    # print(f"Left point1:  {left_chart.GetAxis(2).GetPoint1()}")
    # print(f"Left point1:  {left_chart.GetAxis(1).GetPoint2()}")
    # print(f"Right point1: {right_chart.GetPoint1()}")  # Equals chart_right.GetAxis(1).GetPoint1() instead of (330,10)?
    # print(f"Right point2: {right_chart.GetPoint2()}")  # Equals chart_right.GetAxis(1).GetPoint1() instead of (330,10)?
    iren.Initialize()
    iren.Start()


if __name__ == '__main__':
    main()
