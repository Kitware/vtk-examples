#!/usr/bin/env python

import vtk


def main(argv):
    reader = vtk.vtkXMLUnstructuredGridReader()
    reader.SetFileName(argv[1])
    reader.Update()

    converter = vtk.vtkUnstructuredGridToExplicitStructuredGrid()
    converter.GlobalWarningDisplayOff()  # hide VTK errors
    converter.SetInputConnection(reader.GetOutputPort())
    converter.SetInputArrayToProcess(0, 0, 0, 1, 'BLOCK_I')
    converter.SetInputArrayToProcess(1, 0, 0, 1, 'BLOCK_J')
    converter.SetInputArrayToProcess(2, 0, 0, 1, 'BLOCK_K')
    converter.Update()

    grid = converter.GetOutput()
    grid.ComputeFacesConnectivityFlagsArray()
    grid.GetCellData().SetActiveScalars('ConnectivityFlags')

    scalars = grid.GetCellData().GetArray('ConnectivityFlags')

    mapper = vtk.vtkDataSetMapper()
    mapper.SetInputData(grid)
    mapper.SetColorModeToMapScalars()
    mapper.SetScalarRange(scalars.GetRange())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().EdgeVisibilityOn()

    colors = vtk.vtkNamedColors()

    renderer = vtk.vtkRenderer()
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('DimGray'))

    window = vtk.vtkRenderWindow()
    window.AddRenderer(renderer)
    window.SetWindowName('LoadESGrid')
    window.SetSize(1024, 768)

    interactor = vtk.vtkRenderWindowInteractor()
    interactor.SetRenderWindow(window)
    interactor.SetInteractorStyle(vtk.vtkInteractorStyleRubberBandPick())
    interactor.Start()


if __name__ == '__main__':
    import sys
    main(sys.argv)
