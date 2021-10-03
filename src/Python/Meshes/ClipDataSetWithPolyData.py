#!/usr/bin/env python

import numpy as np
# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkFloatArray
from vtkmodules.vtkCommonDataModel import (
    vtkCellTypes,
    vtkRectilinearGrid
)
from vtkmodules.vtkFiltersCore import vtkImplicitPolyDataDistance
from vtkmodules.vtkFiltersGeneral import vtkClipDataSet
from vtkmodules.vtkFiltersGeometry import vtkRectilinearGridGeometryFilter
from vtkmodules.vtkFiltersSources import vtkConeSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create polydata to slice the grid with. In this case, use a cone. This
    # could
    # be any polydata including a stl file.
    cone = vtkConeSource()
    cone.SetResolution(50)
    cone.SetDirection(0, 0, -1)
    cone.SetHeight(3.0)
    cone.CappingOn()
    cone.Update()

    # Implicit function that will be used to slice the mesh
    implicitPolyDataDistance = vtkImplicitPolyDataDistance()
    implicitPolyDataDistance.SetInput(cone.GetOutput())

    # create a grid
    dimension = 51
    xCoords = vtkFloatArray()
    for x, i in enumerate(np.linspace(-1.0, 1.0, dimension)):
        xCoords.InsertNextValue(i)

    yCoords = vtkFloatArray()
    for y, i in enumerate(np.linspace(-1.0, 1.0, dimension)):
        yCoords.InsertNextValue(i)

    zCoords = vtkFloatArray()
    for z, i in enumerate(np.linspace(-1.0, 1.0, dimension)):
        zCoords.InsertNextValue(i)

    # # create a grid - if not using numpy
    # dimension = 51
    # xCoords = vtkFloatArray()
    # for i in range(0, dimension):
    #     xCoords.InsertNextValue(-1.0 + i * 2.0 / (dimension - 1))
    #
    # yCoords = vtkFloatArray()
    # for i in range(0, dimension):
    #     yCoords.InsertNextValue(-1.0 + i * 2.0 / (dimension - 1))
    #
    # zCoords = vtkFloatArray()
    # for i in range(0, dimension):
    #     zCoords.InsertNextValue(-1.0 + i * 2.0 / (dimension - 1))

    # The coordinates are assigned to the rectilinear grid. Make sure that
    # the number of values in each of the XCoordinates, YCoordinates,
    # and ZCoordinates is equal to what is defined in SetDimensions().
    rgrid = vtkRectilinearGrid()
    rgrid.SetDimensions(xCoords.GetNumberOfTuples(),
                        yCoords.GetNumberOfTuples(),
                        zCoords.GetNumberOfTuples())
    rgrid.SetXCoordinates(xCoords)
    rgrid.SetYCoordinates(yCoords)
    rgrid.SetZCoordinates(zCoords)

    # Create an array to hold distance information
    signedDistances = vtkFloatArray()
    signedDistances.SetNumberOfComponents(1)
    signedDistances.SetName('SignedDistances')

    # Evaluate the signed distance function at all of the grid points
    for pointId in range(0, rgrid.GetNumberOfPoints()):
        p = rgrid.GetPoint(pointId)
        signedDistance = implicitPolyDataDistance.EvaluateFunction(p)
        signedDistances.InsertNextValue(signedDistance)

    # Add the SignedDistances to the grid
    rgrid.GetPointData().SetScalars(signedDistances)

    # Use vtkClipDataSet to slice the grid with the polydata
    clipper = vtkClipDataSet()
    clipper.SetInputData(rgrid)
    clipper.InsideOutOn()
    clipper.SetValue(0.0)
    clipper.GenerateClippedOutputOn()
    clipper.Update()

    # --- mappers, actors, render, etc. ---
    # mapper and actor to view the cone
    coneMapper = vtkPolyDataMapper()
    coneMapper.SetInputConnection(cone.GetOutputPort())
    coneActor = vtkActor()
    coneActor.SetMapper(coneMapper)

    # geometry filter to view the background grid
    geometryFilter = vtkRectilinearGridGeometryFilter()
    geometryFilter.SetInputData(rgrid)
    geometryFilter.SetExtent(0, dimension, 0, dimension, int(dimension / 2), int(dimension / 2))
    geometryFilter.Update()

    rgridMapper = vtkPolyDataMapper()
    rgridMapper.SetInputConnection(geometryFilter.GetOutputPort())
    rgridMapper.SetScalarRange(
        rgrid.GetPointData().GetArray('SignedDistances').GetRange())

    wireActor = vtkActor()
    wireActor.SetMapper(rgridMapper)
    wireActor.GetProperty().SetRepresentationToWireframe()

    # mapper and actor to view the clipped mesh
    clipperMapper = vtkDataSetMapper()
    clipperMapper.SetInputConnection(clipper.GetOutputPort())
    clipperMapper.ScalarVisibilityOff()

    clipperOutsideMapper = vtkDataSetMapper()
    clipperOutsideMapper.SetInputConnection(clipper.GetOutputPort(1))
    clipperOutsideMapper.ScalarVisibilityOff()

    clipperActor = vtkActor()
    clipperActor.SetMapper(clipperMapper)
    clipperActor.GetProperty().SetColor(colors.GetColor3d('Banana'))

    clipperOutsideActor = vtkActor()
    clipperOutsideActor.SetMapper(clipperOutsideMapper)
    clipperOutsideActor.GetProperty().SetColor(
        colors.GetColor3d('Banana'))

    # A renderer and render window
    # Create a renderer, render window, and interactor
    leftViewport = [0.0, 0.0, 0.5, 1.0]
    leftRenderer = vtkRenderer()
    leftRenderer.SetViewport(leftViewport)
    leftRenderer.SetBackground(colors.GetColor3d('SteelBlue'))

    rightViewport = [0.5, 0.0, 1.0, 1.0]
    rightRenderer = vtkRenderer()
    rightRenderer.SetViewport(rightViewport)
    rightRenderer.SetBackground(colors.GetColor3d('CadetBlue'))

    # add the actors
    leftRenderer.AddActor(wireActor)
    leftRenderer.AddActor(clipperActor)
    rightRenderer.AddActor(clipperOutsideActor)

    renwin = vtkRenderWindow()
    renwin.SetSize(640, 480)
    renwin.AddRenderer(leftRenderer)
    renwin.AddRenderer(rightRenderer)
    renwin.SetWindowName('ClipDataSetWithPolyData')

    # An interactor
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renwin)

    # Share the camera

    leftRenderer.GetActiveCamera().SetPosition(0, -1, 0)
    leftRenderer.GetActiveCamera().SetFocalPoint(0, 0, 0)
    leftRenderer.GetActiveCamera().SetViewUp(0, 0, 1)
    leftRenderer.GetActiveCamera().Azimuth(30)
    leftRenderer.GetActiveCamera().Elevation(30)
    leftRenderer.ResetCamera()
    rightRenderer.SetActiveCamera(leftRenderer.GetActiveCamera())

    renwin.Render()
    interactor.Start()

    # Generate a report
    ct = vtkCellTypes()

    numberOfCells = clipper.GetOutput().GetNumberOfCells()
    print('------------------------')
    print('The clipped dataset(inside) contains a\n', clipper.GetOutput().GetClassName(), 'that has', numberOfCells,
          'cells')
    cellMap = dict()
    for i in range(0, numberOfCells):
        cellMap[clipper.GetOutput().GetCellType(i)] = cellMap.get(clipper.GetOutput().GetCellType(i), 0) + 1

    for k, v in cellMap.items():
        print('\tCell type ', ct.GetClassNameFromTypeId(k), 'occurs', v, 'times.')

    numberOfCells = clipper.GetClippedOutput().GetNumberOfCells()
    print('------------------------')
    print('The clipped dataset(outside) contains a\n', clipper.GetClippedOutput().GetClassName(), 'that has',
          numberOfCells, 'cells')
    outsideCellMap = dict()
    for i in range(0, numberOfCells):
        outsideCellMap[clipper.GetClippedOutput().GetCellType(i)] = outsideCellMap.get(
            clipper.GetClippedOutput().GetCellType(i), 0) + 1

    for k, v in outsideCellMap.items():
        print('\tCell type ', ct.GetClassNameFromTypeId(k), 'occurs', v, 'times.')


if __name__ == '__main__':
    main()
