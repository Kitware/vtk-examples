import math

import vtk


def main():
    colors = vtk.vtkNamedColors()

    renwin = vtk.vtkRenderWindow()
    renwin.SetWindowName('MultipleGraphs')
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(renwin)

    # Setup the viewports
    grid_dimensions_x = 2
    grid_dimensions_y = 1
    renderer_sz_x = 320
    renderer_sz_y = 240
    renwin.SetSize(renderer_sz_x * grid_dimensions_x, renderer_sz_y * grid_dimensions_y)

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

    # Create the charts.
    left_chart_scene = vtk.vtkContextScene()
    left_chart_actor = vtk.vtkContextActor()
    left_chart_actor.SetScene(left_chart_scene)
    left_renderer.AddActor(left_chart_actor)
    left_chart_scene.SetRenderer(left_renderer)

    left_chart = vtk.vtkChartXY()
    x_axis = left_chart.GetAxis(vtk.vtkAxis.BOTTOM)
    x_axis.GetGridPen().SetColor(colors.GetColor4ub("LightGrey"))
    x_axis.SetTitle('x')
    y_axis = left_chart.GetAxis(vtk.vtkAxis.LEFT)
    y_axis.GetGridPen().SetColor(colors.GetColor4ub("LightGrey"))
    y_axis.SetTitle('cos(x)')
    left_chart.GetBackgroundBrush().SetColorF(*colors.GetColor4d('MistyRose'))
    left_chart.GetBackgroundBrush().SetOpacityF(0.4)
    left_chart.SetTitle('Cosine')
    left_chart_scene.AddItem(left_chart)

    right_chart_scene = vtk.vtkContextScene()
    right_chart_actor = vtk.vtkContextActor()
    right_chart_actor.SetScene(right_chart_scene)
    right_renderer.AddActor(right_chart_actor)
    right_chart = vtk.vtkChartXY()
    x_axis = right_chart.GetAxis(vtk.vtkAxis.BOTTOM)
    x_axis.GetGridPen().SetColor(colors.GetColor4ub("LightCyan"))
    x_axis.SetTitle('x')
    y_axis = right_chart.GetAxis(vtk.vtkAxis.LEFT)
    y_axis.GetGridPen().SetColor(colors.GetColor4ub("LightCyan"))
    y_axis.SetTitle('sin(x)')
    right_chart.GetBackgroundBrush().SetColorF(*colors.GetColor4d('Thistle'))
    right_chart.GetBackgroundBrush().SetOpacityF(0.4)
    right_chart.SetTitle('Sine')
    right_chart_scene.AddItem(right_chart)

    # Create the data.
    table = vtk.vtkTable()
    array_x = vtk.vtkFloatArray()
    array_x.SetName('X Axis')
    table.AddColumn(array_x)

    array_cos = vtk.vtkFloatArray()
    array_cos.SetName('Cosine')
    table.AddColumn(array_cos)

    array_sin = vtk.vtkFloatArray()
    array_sin.SetName('Sine')
    table.AddColumn(array_sin)

    # Fill in the table with some example values.
    num_points = 40
    inc = 7.5 / (num_points - 1)
    table.SetNumberOfRows(num_points)
    for i in range(num_points):
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

    renwin.Render()
    iren.Initialize()
    iren.Start()


if __name__ == '__main__':
    main()
