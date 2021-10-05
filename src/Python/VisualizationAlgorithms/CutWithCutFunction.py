#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkPlane
from vtkmodules.vtkFiltersCore import vtkCutter
from vtkmodules.vtkIOXML import vtkXMLPolyDataReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    fileName, numberOfCuts = get_program_parameters()
    reader = vtkXMLPolyDataReader()
    reader.SetFileName(fileName)
    reader.Update()

    bounds = reader.GetOutput().GetBounds()
    print('Bounds:', ', '.join(['{:.3f}'.format(f) for f in bounds]))

    plane = vtkPlane()
    plane.SetOrigin((bounds[1] + bounds[0]) / 2.0, (bounds[3] + bounds[2]) / 2.0, bounds[4])
    plane.SetNormal(0, 0, 1)

    # Create cutter
    high = plane.EvaluateFunction((bounds[1] + bounds[0]) / 2.0, (bounds[3] + bounds[2]) / 2.0, bounds[5])

    cutter = vtkCutter()
    cutter.SetInputConnection(reader.GetOutputPort())
    cutter.SetCutFunction(plane)
    cutter.GenerateValues(numberOfCuts, 0.99, 0.99 * high)

    cutterMapper = vtkPolyDataMapper()
    cutterMapper.SetInputConnection(cutter.GetOutputPort())
    cutterMapper.ScalarVisibilityOff()

    # Create cut actor
    cutterActor = vtkActor()
    cutterActor.GetProperty().SetColor(colors.GetColor3d('Banana'))
    cutterActor.GetProperty().SetLineWidth(2)
    cutterActor.SetMapper(cutterMapper)

    # Create model actor
    modelMapper = vtkPolyDataMapper()
    modelMapper.SetInputConnection(reader.GetOutputPort())
    modelMapper.ScalarVisibilityOff()

    modelActor = vtkActor()
    modelActor.GetProperty().SetColor(colors.GetColor3d('Flesh'))
    modelActor.SetMapper(modelMapper)

    # Create renderers and add actors of plane and model
    renderer = vtkRenderer()
    renderer.AddActor(cutterActor)
    renderer.AddActor(modelActor)

    # Add renderer to renderwindow and render
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetSize(600, 600)
    renderWindow.SetWindowName('CutWithCutFunction')

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    renderer.SetBackground(colors.GetColor3d('Burlywood'))
    renderer.GetActiveCamera().SetPosition(0, -1, 0)
    renderer.GetActiveCamera().SetFocalPoint(0, 0, 0)
    renderer.GetActiveCamera().SetViewUp(0, 0, 1)
    renderer.GetActiveCamera().Azimuth(30)
    renderer.GetActiveCamera().Elevation(30)

    renderer.ResetCamera()
    renderWindow.Render()

    interactor.Start()


def get_program_parameters():
    import argparse
    description = 'Cut with a cut function.'
    epilogue = '''
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='Torso.vtp')
    parser.add_argument('numberOfCuts', default=10, type=int, nargs='?', help='The number of cuts e.g 10.')
    args = parser.parse_args()
    return args.filename, args.numberOfCuts


if __name__ == '__main__':
    main()
