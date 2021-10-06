#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkContourFilter
from vtkmodules.vtkFiltersHybrid import vtkImplicitModeller
from vtkmodules.vtkIOLegacy import vtkPolyDataReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    fileName = get_program_parameters()

    colors = vtkNamedColors()

    # Create lines which serve as the 'seed' geometry. The lines spell the
    # word 'hello'.
    #
    reader = vtkPolyDataReader()
    reader.SetFileName(fileName)

    lineMapper = vtkPolyDataMapper()
    lineMapper.SetInputConnection(reader.GetOutputPort())

    lineActor = vtkActor()
    lineActor.SetMapper(lineMapper)
    lineActor.GetProperty().SetColor(colors.GetColor3d('Tomato'))
    lineActor.GetProperty().SetLineWidth(3.0)

    # Create implicit model with vtkImplicitModeller. This computes a scalar
    # field which is the distance from the generating geometry. The contour
    # filter then extracts the geometry at the distance value 0.25 from the
    # generating geometry.
    #
    imp = vtkImplicitModeller()
    imp.SetInputConnection(reader.GetOutputPort())
    imp.SetSampleDimensions(110, 40, 20)
    imp.SetMaximumDistance(0.25)
    imp.SetModelBounds(-1.0, 10.0, -1.0, 3.0, -1.0, 1.0)

    contour = vtkContourFilter()
    contour.SetInputConnection(imp.GetOutputPort())
    contour.SetValue(0, 0.25)

    impMapper = vtkPolyDataMapper()
    impMapper.SetInputConnection(contour.GetOutputPort())
    impMapper.ScalarVisibilityOff()

    impActor = vtkActor()
    impActor.SetMapper(impMapper)
    impActor.GetProperty().SetColor(colors.GetColor3d('Peacock'))
    impActor.GetProperty().SetOpacity(0.5)

    # Create the usual graphics stuff.
    # Create the RenderWindow, Renderer and Interactor.
    #
    ren1 = vtkRenderer()

    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren1)
    renWin.SetWindowName('Hello')

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Add the actors to the renderer, set the background and size
    #
    ren1.AddActor(lineActor)
    ren1.AddActor(impActor)
    ren1.SetBackground(colors.GetColor3d('Wheat'))
    renWin.SetSize(640, 480)

    camera = vtkCamera()
    camera.SetFocalPoint(4.5, 1, 0)
    camera.SetPosition(4.5, 1.0, 6.73257)
    camera.SetViewUp(0, 1, 0)

    ren1.SetActiveCamera(camera)
    ren1.ResetCamera()
    camera.Dolly(1.3)
    camera.SetClippingRange(1.81325, 90.6627)

    renWin.Render()
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'Implicit modelling is used to thicken a stroked font.'
    epilogue = '''
    Implicit modelling is used to thicken a stroked font.
     The original lines can be seen within the translucent implicit surface. 
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='hello.vtk')
    args = parser.parse_args()
    return args.filename


if __name__ == '__main__':
    main()
