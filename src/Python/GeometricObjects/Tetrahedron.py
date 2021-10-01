#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonDataModel import (
    VTK_TETRA,
    vtkCellArray,
    vtkTetra,
    vtkUnstructuredGrid
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(1, 0, 0)
    points.InsertNextPoint(1, 1, 0)
    points.InsertNextPoint(0, 1, 1)

    points.InsertNextPoint(2, 2, 2)
    points.InsertNextPoint(3, 2, 2)
    points.InsertNextPoint(3, 3, 2)
    points.InsertNextPoint(2, 3, 3)

    # The first tetrahedron
    unstructuredGrid1 = vtkUnstructuredGrid()
    unstructuredGrid1.SetPoints(points)

    tetra = vtkTetra()

    tetra.GetPointIds().SetId(0, 0)
    tetra.GetPointIds().SetId(1, 1)
    tetra.GetPointIds().SetId(2, 2)
    tetra.GetPointIds().SetId(3, 3)

    cellArray = vtkCellArray()
    cellArray.InsertNextCell(tetra)
    unstructuredGrid1.SetCells(VTK_TETRA, cellArray)

    # The second tetrahedron
    unstructuredGrid2 = vtkUnstructuredGrid()
    unstructuredGrid2.SetPoints(points)

    tetra = vtkTetra()

    tetra.GetPointIds().SetId(0, 4)
    tetra.GetPointIds().SetId(1, 5)
    tetra.GetPointIds().SetId(2, 6)
    tetra.GetPointIds().SetId(3, 7)

    cellArray = vtkCellArray()
    cellArray.InsertNextCell(tetra)
    unstructuredGrid2.SetCells(VTK_TETRA, cellArray)

    # Create a mapper and actor
    mapper1 = vtkDataSetMapper()
    mapper1.SetInputData(unstructuredGrid1)

    actor1 = vtkActor()
    actor1.SetMapper(mapper1)
    actor1.GetProperty().SetColor(colors.GetColor3d("Cyan"))

    # Create a mapper and actor
    mapper2 = vtkDataSetMapper()
    mapper2.SetInputData(unstructuredGrid2)

    actor2 = vtkActor()
    actor2.SetMapper(mapper2)
    actor2.GetProperty().SetColor(colors.GetColor3d("Yellow"))

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName("Tetrahedron")
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actor to the scene
    renderer.AddActor(actor1)
    renderer.AddActor(actor2)
    renderer.SetBackground(colors.GetColor3d("DarkGreen"))
    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(-10)
    renderer.GetActiveCamera().Elevation(-20)

    # Render and interact
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
