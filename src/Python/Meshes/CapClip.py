#!/usr/bin/env python

import os.path

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import (
    vtkPlane,
    vtkPolyData
)
from vtkmodules.vtkFiltersCore import (
    vtkClipPolyData,
    vtkFeatureEdges,
    vtkStripper
)
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkIOGeometry import (
    vtkBYUReader,
    vtkOBJReader,
    vtkSTLReader
)
from vtkmodules.vtkIOPLY import vtkPLYReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def get_program_parameters():
    import argparse
    description = 'Clip polydata using a plane.'
    epilogue = '''
    This is an example using vtkClipPolyData to clip input polydata, if provided, or a sphere otherwise.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename', nargs='?', default=None, help='Optional input filename e.g cow.g.')
    args = parser.parse_args()
    return args.filename


def main():
    filePath = get_program_parameters()

    # Define colors
    colors = vtkNamedColors()
    backgroundColor = colors.GetColor3d('steel_blue')
    boundaryColor = colors.GetColor3d('Banana')
    clipColor = colors.GetColor3d('Tomato')

    if filePath and os.path.isfile(filePath):
        polyData = ReadPolyData(filePath)
        if not polyData:
            polyData = GetSpherePD()
    else:
        polyData = GetSpherePD()

    plane = vtkPlane()
    plane.SetOrigin(polyData.GetCenter())
    plane.SetNormal(1.0, -1.0, -1.0)

    clipper = vtkClipPolyData()
    clipper.SetInputData(polyData)
    clipper.SetClipFunction(plane)
    clipper.SetValue(0)
    clipper.Update()

    polyData = clipper.GetOutput()

    clipMapper = vtkDataSetMapper()
    clipMapper.SetInputData(polyData)

    clipActor = vtkActor()
    clipActor.SetMapper(clipMapper)
    clipActor.GetProperty().SetDiffuseColor(clipColor)
    clipActor.GetProperty().SetInterpolationToFlat()
    clipActor.GetProperty().EdgeVisibilityOn()

    # Now extract feature edges
    boundaryEdges = vtkFeatureEdges()
    boundaryEdges.SetInputData(polyData)
    boundaryEdges.BoundaryEdgesOn()
    boundaryEdges.FeatureEdgesOff()
    boundaryEdges.NonManifoldEdgesOff()
    boundaryEdges.ManifoldEdgesOff()

    boundaryStrips = vtkStripper()
    boundaryStrips.SetInputConnection(boundaryEdges.GetOutputPort())
    boundaryStrips.Update()

    # Change the polylines into polygons
    boundaryPoly = vtkPolyData()
    boundaryPoly.SetPoints(boundaryStrips.GetOutput().GetPoints())
    boundaryPoly.SetPolys(boundaryStrips.GetOutput().GetLines())

    boundaryMapper = vtkPolyDataMapper()
    boundaryMapper.SetInputData(boundaryPoly)

    boundaryActor = vtkActor()
    boundaryActor.SetMapper(boundaryMapper)
    boundaryActor.GetProperty().SetDiffuseColor(boundaryColor)

    # create renderer render window, and interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    # set background color and size
    renderer.SetBackground(backgroundColor)
    renderWindow.SetSize(640, 480)

    # add our actor to the renderer
    renderer.AddActor(clipActor)
    renderer.AddActor(boundaryActor)

    # Generate an interesting view
    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(30)
    renderer.GetActiveCamera().Elevation(30)
    renderer.GetActiveCamera().Dolly(1.2)
    renderer.ResetCameraClippingRange()

    renderWindow.Render()
    renderWindow.SetWindowName('CapClip')
    renderWindow.Render()

    interactor.Start()


def ReadPolyData(file_name):
    import os
    path, extension = os.path.splitext(file_name)
    extension = extension.lower()
    if extension == '.ply':
        reader = vtkPLYReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == '.vtp':
        reader = vtkXMLpoly_dataReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == '.obj':
        reader = vtkOBJReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == '.stl':
        reader = vtkSTLReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == '.vtk':
        reader = vtkpoly_dataReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == '.g':
        reader = vtkBYUReader()
        reader.SetGeometryFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    else:
        # Return a None if the extension is unknown.
        poly_data = None
    return poly_data


def GetSpherePD():
    '''
    :return: The PolyData representation of a sphere.
    '''
    source = vtkSphereSource()
    source.SetThetaResolution(20)
    source.SetPhiResolution(11)
    source.Update()
    return source.GetOutput()


if __name__ == '__main__':
    main()
