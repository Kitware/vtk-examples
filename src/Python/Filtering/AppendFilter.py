#!/usr/bin/env python

import vtk


def main():
    colors = vtk.vtkNamedColors()

    # Create 5 points (vtkPolyData)
    pointSource = vtk.vtkPointSource()
    pointSource.SetNumberOfPoints(5)
    pointSource.Update()

    polydata = pointSource.GetOutput()

    print('There are', polydata.GetNumberOfPoints(), 'points in the polydata.')

    # Create 2 points in a vtkUnstructuredGrid
    points = vtk.vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(0, 0, 1)

    ug = vtk.vtkUnstructuredGrid()
    ug.SetPoints(points)
    print('There are', ug.GetNumberOfPoints(), 'points in the unstructured.')

    # Combine the two data sets
    appendFilter = vtk.vtkAppendFilter()
    appendFilter.AddInputData(polydata)
    appendFilter.AddInputData(ug)
    appendFilter.Update()

    combined = vtk.vtkUnstructuredGrid()

    combined = appendFilter.GetOutput()
    print('There are', combined.GetNumberOfPoints(), 'points combined.')

    # Create a mapper and actor
    mapper = vtk.vtkDataSetMapper()
    mapper.SetInputConnection(appendFilter.GetOutputPort())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetPointSize(5)

    # Create a renderer, render window, and interactor
    renderer = vtk.vtkRenderer()
    renderWindow = vtk.vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtk.vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actor to the scene
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))

    # Render and interact
    renderWindow.SetWindowName('AppendFilter')
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
