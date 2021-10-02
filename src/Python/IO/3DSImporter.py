#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkIOImport import vtk3DSImporter
from vtkmodules.vtkRenderingCore import (
    vtkCamera,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    fileName = get_program_parameters()

    importer = vtk3DSImporter()
    importer.SetFileName(fileName)
    importer.ComputeNormalsOn()

    colors = vtkNamedColors()

    renderer = vtkRenderer()
    renWin = vtkRenderWindow()
    iren = vtkRenderWindowInteractor()

    renWin.AddRenderer(renderer)
    renderer.SetBackground2(colors.GetColor3d('Gold'))
    renderer.SetBackground(colors.GetColor3d('Wheat'))
    renderer.GradientBackgroundOn()

    iren.SetRenderWindow(renWin)
    importer.SetRenderWindow(renWin)
    importer.Update()

    # actors = vtkActorCollection()
    actors = renderer.GetActors()
    print('There are', actors.GetNumberOfItems(), 'actors.')

    renWin.SetWindowName('3DSImporter')
    renWin.Render()
    camera = vtkCamera()
    camera.SetPosition(0, -1, 0)
    camera.SetFocalPoint(0, 0, 0)
    camera.SetViewUp(0, 0, 1)
    camera.Azimuth(150)
    camera.Elevation(30)

    renderer.SetActiveCamera(camera)
    renderer.ResetCamera()
    renderer.ResetCameraClippingRange()

    renWin.Render()
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'Importing a 3ds file.'
    epilogue = '''
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='iflamingo.3ds.')
    args = parser.parse_args()
    return args.filename


if __name__ == '__main__':
    main()
