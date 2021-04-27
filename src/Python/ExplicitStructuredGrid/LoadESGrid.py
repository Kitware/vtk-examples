#!/usr/bin/env python

import vtk


def get_program_parameters(argv):
    import argparse
    description = 'Load an explicit structured grid from a file'
    epilogue = '''
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('fn', help='The explicit structured grid file name e.g. UNISIM-II-D.vtu.')
    args = parser.parse_args()
    return args.fn


def main(fn):
    reader = vtk.vtkXMLUnstructuredGridReader()
    reader.SetFileName(fn)
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
    window.Render()

    camera = renderer.GetActiveCamera()
    camera.SetPosition(312452.407650, 7474760.406373, 3507.364723)
    camera.SetFocalPoint(314388.388434, 7481520.509575, -2287.477388)
    camera.SetViewUp(0.089920, 0.633216, 0.768734)
    camera.SetDistance(9111.926908)
    camera.SetClippingRange(595.217338, 19595.429475)

    interactor = vtk.vtkRenderWindowInteractor()
    interactor.SetRenderWindow(window)
    interactor.SetInteractorStyle(vtk.vtkInteractorStyleRubberBandPick())
    window.Render()
    interactor.Start()


if __name__ == '__main__':
    import sys

    fn = get_program_parameters(sys.argv)
    main(fn)
