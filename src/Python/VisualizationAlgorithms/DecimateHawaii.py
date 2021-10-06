#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import (
    vtkDecimatePro,
    vtkPolyDataNormals
)
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
    colors = vtkNamedColors()

    fileName = get_program_parameters()

    # This example shows how to use decimation to reduce a polygonal mesh. We also
    # use mesh smoothing and generate surface normals to give a pleasing result.
    #

    hawaii = vtkPolyDataReader()
    hawaii.SetFileName(fileName)

    # We want to preserve topology (not let any cracks form). This may limit
    # the total reduction possible, which we have specified at 90%.
    #
    deci = vtkDecimatePro()
    deci.SetInputConnection(hawaii.GetOutputPort())
    deci.SetTargetReduction(0.9)
    deci.PreserveTopologyOn()

    decimatedNormals = vtkPolyDataNormals()
    decimatedNormals.SetInputConnection(deci.GetOutputPort())
    decimatedNormals.FlipNormalsOn()
    decimatedNormals.SetFeatureAngle(60)

    decimatedMapper = vtkPolyDataMapper()
    decimatedMapper.SetInputConnection(decimatedNormals.GetOutputPort())

    decimatedActor = vtkActor()
    decimatedActor.SetMapper(decimatedMapper)
    decimatedActor.GetProperty().SetColor(colors.GetColor3d('Sienna'))
    decimatedActor.GetProperty().SetRepresentationToWireframe()

    originalMapper = vtkPolyDataMapper()
    originalMapper.SetInputConnection(decimatedNormals.GetOutputPort())

    originalActor = vtkActor()
    originalActor.SetMapper(originalMapper)
    originalActor.GetProperty().SetColor(colors.GetColor3d('Sienna'))

    # Create the RenderWindow, Renderer and Interactor.
    #
    renderer1 = vtkRenderer()
    renderer1.SetViewport(0.0, 0.0, 0.5, 1.0)

    renderer2 = vtkRenderer()
    renderer2.SetViewport(0.5, 0.0, 1.0, 1.0)

    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer1)
    renderWindow.AddRenderer(renderer2)

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    # Add the actors to the renderer, set the background and size.
    #
    renderer1.AddActor(originalActor)
    renderer2.AddActor(decimatedActor)
    renderer1.SetBackground(colors.GetColor3d('Wheat'))
    renderer2.SetBackground(colors.GetColor3d('Papaya_Whip'))
    renderWindow.SetSize(800, 400)
    renderWindow.SetWindowName('DecimateHawaii')

    # Render the image.
    #
    cam1 = vtkCamera()
    renderer1.SetActiveCamera(cam1)
    renderer2.SetActiveCamera(cam1)
    renderer1.ResetCamera()
    cam1.Elevation(-30)
    cam1.Dolly(1.2)
    renderer1.ResetCameraClippingRange()
    renderWindow.Render()
    interactor.Start()


def get_program_parameters():
    import argparse
    description = 'Examples of decimation algorithm.'
    epilogue = '''
    Decimation of terrain data.
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename1', help='honolulu.vtk')
    args = parser.parse_args()
    return args.filename1


if __name__ == '__main__':
    main()
