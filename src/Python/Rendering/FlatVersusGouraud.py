#!/usr/bin/env python
# -*- coding: utf-8 -*-

from collections import OrderedDict

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkQuadric
from vtkmodules.vtkFiltersCore import vtkContourFilter
from vtkmodules.vtkFiltersSources import (
    vtkCylinderSource,
    vtkSphereSource
)
from vtkmodules.vtkIOGeometry import vtkOBJReader
from vtkmodules.vtkImagingHybrid import vtkSampleFunction
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    fileName = get_program_parameters()

    # Create the renderers.
    renderers = OrderedDict()
    renderers['flatSphereRenderer'] = CreateSphere(True)
    renderers['flatCylinderRenderer'] = CreateCylinder(True)
    renderers['flatIsoSurfaceRenderer'] = CreateIsoSurface(True)
    renderers['flatModelRenderer'] = CreateModel(True, fileName)

    renderers['smoothSphereRenderer'] = CreateSphere(False)
    renderers['smoothCylinderRenderer'] = CreateCylinder(False)
    renderers['smoothIsoSurfaceRenderer'] = CreateIsoSurface(False)
    renderers['smoothModelRenderer'] = CreateModel(False, fileName)

    # Get the keys
    keys = list(renderers.keys())

    renderWindow = vtkRenderWindow()

    # Setup the viewports for the renderers.
    rendererSize = 256
    xGridDimensions = 4
    yGridDimensions = 2

    renderWindow.SetSize(rendererSize * xGridDimensions, rendererSize * yGridDimensions)
    renderWindow.SetWindowName('FlatVersusGouraud')

    for row in range(0, yGridDimensions):
        for col in range(0, xGridDimensions):
            # (xmin, ymin, xmax, ymax)
            viewport = [0] * 4
            viewport[0] = col / xGridDimensions
            viewport[1] = (yGridDimensions - (row + 1)) / yGridDimensions
            viewport[2] = (col + 1) / xGridDimensions
            viewport[3] = (yGridDimensions - row) / yGridDimensions
            index = row * xGridDimensions + col
            renderers[keys[index]].SetViewport(viewport)

    for r in range(0, len(renderers)):
        renderers[keys[r]].SetBackground(colors.GetColor3d('SlateGray'))
        renderers[keys[r]].GetActiveCamera().Azimuth(20)
        renderers[keys[r]].GetActiveCamera().Elevation(30)
        renderers[keys[r]].ResetCamera()
        if r > 3:
            renderers[keys[r]].SetActiveCamera(renderers[keys[r - 4]].GetActiveCamera())

        renderWindow.AddRenderer(renderers[keys[r]])

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    renderWindow.Render()
    interactor.Start()


def get_program_parameters():
    import argparse
    description = 'Flat and Gouraud shading.'
    epilogue = '''
    Flat and Gouraud shading.
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='cow.obj.')
    args = parser.parse_args()
    return args.filename


def CreateSphere(flat):
    '''

    :param flat: The interpolation to use (flat or Gouraud).
    :return: the renderer
    '''
    colors = vtkNamedColors()
    sphere = vtkSphereSource()
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(sphere.GetOutputPort())
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))
    if flat:
        actor.GetProperty().SetInterpolationToFlat()
    else:
        actor.GetProperty().SetInterpolationToGouraud()
    renderer = vtkRenderer()
    renderer.AddActor(actor)
    return renderer


def CreateCylinder(flat):
    '''

    :param flat: The interpolation to use (flat or Gouraud).
    :return: the renderer
    '''
    colors = vtkNamedColors()
    cylinder = vtkCylinderSource()
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(cylinder.GetOutputPort())
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))
    if flat:
        actor.GetProperty().SetInterpolationToFlat()
    else:
        actor.GetProperty().SetInterpolationToGouraud()
    renderer = vtkRenderer()
    renderer.AddActor(actor)
    return renderer


def CreateIsoSurface(flat):
    '''

    :param flat: The interpolation to use (flat or Gouraud).
    :return: the renderer
    '''
    quadric = vtkQuadric()
    quadric.SetCoefficients(1, 2, 3, 0, 1, 0, 0, 0, 0, 0)
    sample = vtkSampleFunction()
    sample.SetSampleDimensions(25, 25, 25)
    sample.SetImplicitFunction(quadric)
    # Generate the implicit surface.
    contour = vtkContourFilter()
    contour.SetInputConnection(sample.GetOutputPort())
    range = [1.0, 6.0]
    contour.GenerateValues(5, range)
    # Map the contour.
    contourMapper = vtkPolyDataMapper()
    contourMapper.SetInputConnection(contour.GetOutputPort())
    contourMapper.SetScalarRange(0, 7)
    actor = vtkActor()
    actor.SetMapper(contourMapper)
    if flat:
        actor.GetProperty().SetInterpolationToFlat()
    else:
        actor.GetProperty().SetInterpolationToGouraud()
    renderer = vtkRenderer()
    renderer.AddActor(actor)
    return renderer


def CreateModel(flat, fileName):
    '''

    :param flat: The interpolation to use (flat or Gouraud).
    :param fileName: The file name.
    :return: the renderer
    '''
    colors = vtkNamedColors()
    reader = vtkOBJReader()
    reader.SetFileName(fileName)
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(reader.GetOutputPort())
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('Tan'))
    if flat:
        actor.GetProperty().SetInterpolationToFlat()
    else:
        actor.GetProperty().SetInterpolationToGouraud()
    renderer = vtkRenderer()
    renderer.AddActor(actor)
    return renderer


if __name__ == '__main__':
    main()
