#!/usr/bin/env python3

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonDataModel import vtkStructuredGrid
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    structured_grid = vtkStructuredGrid()

    points = vtkPoints()

    grid_size = 8
    counter = 0
    # Create a 5x5 grid of points
    for j in range(0, grid_size):
        for i in range(0, grid_size):
            if i == 3 and j == 3:  # Make one point higher than the rest.
                points.InsertNextPoint(i, j, 2)
                print('The different point is number ', counter)
            else:
                # Make most of the points the same height.
                points.InsertNextPoint(i, j, 0)
            counter += 1

    # Specify the dimensions of the grid.
    structured_grid.SetDimensions(grid_size, grid_size, 1)

    structured_grid.SetPoints(points)

    structured_grid.BlankPoint(27)
    structured_grid.Modified()

    # Check.
    def is_visible(pt_num):
        if structured_grid.IsPointVisible(pt_num):
            return f'Point {pt_num} is visible.'
        else:
            return f'Point {pt_num} is not visible.'

    # Should not be visible.
    print(is_visible(27))
    # Should be visible.
    print(is_visible(7))

    # Create a mapper and actor.
    grid_mapper = vtkDataSetMapper()
    grid_mapper.SetInputData(structured_grid)

    grid_actor = vtkActor()
    grid_actor.SetMapper(grid_mapper)
    grid_actor.GetProperty().EdgeVisibilityOn()
    grid_actor.GetProperty().SetEdgeColor(colors.GetColor3d('Blue'))

    # Visualize
    renderer = vtkRenderer()
    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(renderer)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    renderer.AddActor(grid_actor)
    renderer.SetBackground(colors.GetColor3d('ForestGreen'))

    # ren_win.SetSize(640, 480)
    ren_win.SetWindowName('BlankPoint')
    ren_win.Render()
    iren.Start()


if __name__ == '__main__':
    main()
