#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import (
    vtkBiQuadraticQuad,
    vtkBiQuadraticQuadraticHexahedron,
    vtkBiQuadraticQuadraticWedge,
    vtkBiQuadraticTriangle,
    vtkCubicLine,
    vtkQuadraticEdge,
    vtkQuadraticHexahedron,
    vtkQuadraticLinearQuad,
    vtkQuadraticLinearWedge,
    vtkQuadraticPolygon,
    vtkQuadraticPyramid,
    vtkQuadraticQuad,
    vtkQuadraticTetra,
    vtkQuadraticTriangle,
    vtkQuadraticWedge,
    vtkTriQuadraticHexahedron,
    vtkUnstructuredGrid
)
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkActor2D,
    vtkDataSetMapper,
    vtkGlyph3DMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTextMapper,
    vtkTextProperty
)
from vtkmodules.vtkRenderingLabel import vtkLabeledDataMapper


def main():
    titles = list()
    textMappers = list()
    textActors = list()

    uGrids = list()
    mappers = list()
    actors = list()
    renderers = list()

    uGrids.append(MakeUnstructuredGrid(
        vtkQuadraticEdge()))
    titles.append('VTK_QUADRATIC_EDGE (= 21)')

    uGrids.append(MakeUnstructuredGrid(
        vtkQuadraticTriangle()))
    titles.append('VTK_QUADRATIC_TRIANGLE (= 22)')

    uGrids.append(MakeUnstructuredGrid(
        vtkQuadraticQuad()))
    titles.append('VTK_QUADRATIC_QUAD (= 23)')

    uGrids.append(MakeQuadraticPolygon())
    titles.append('VTK_QUADRATIC_POLYGON (= 36)')

    uGrids.append(MakeUnstructuredGrid(
        vtkQuadraticTetra()))
    titles.append('VTK_QUADRATIC_TETRA (= 24)')

    uGrids.append(MakeUnstructuredGrid(
        vtkQuadraticHexahedron()))
    titles.append('VTK_QUADRATIC_HEXAHEDRON (= 25)')

    uGrids.append(MakeUnstructuredGrid(
        vtkQuadraticWedge()))
    titles.append('VTK_QUADRATIC_WEDGE (= 26)')

    uGrids.append(MakeUnstructuredGrid(
        vtkQuadraticPyramid()))
    titles.append('VTK_QUADRATIC_PYRAMID (= 27)')

    uGrids.append(MakeUnstructuredGrid(
        vtkBiQuadraticQuad()))
    titles.append('VTK_BIQUADRATIC_QUAD (= 28)')

    uGrids.append(MakeUnstructuredGrid(
        vtkTriQuadraticHexahedron()))
    titles.append('VTK_TRIQUADRATIC_HEXAHEDRON (= 29)')

    uGrids.append(MakeUnstructuredGrid(
        vtkQuadraticLinearQuad()))
    titles.append('VTK_QUADRATIC_LINEAR_QUAD (= 30)')

    uGrids.append(MakeUnstructuredGrid(
        vtkQuadraticLinearWedge()))
    titles.append('VTK_QUADRATIC_LINEAR_WEDGE (= 31)')

    uGrids.append(MakeUnstructuredGrid(
        vtkBiQuadraticQuadraticWedge()))
    titles.append('VTK_BIQUADRATIC_QUADRATIC_WEDGE (= 32)')

    uGrids.append(MakeUnstructuredGrid(
        vtkBiQuadraticQuadraticHexahedron()))
    titles.append('VTK_BIQUADRATIC_QUADRATIC_HEXAHEDRON (= 33)')

    uGrids.append(MakeUnstructuredGrid(
        vtkBiQuadraticTriangle()))
    titles.append('VTK_BIQUADRATIC_TRIANGLE (= 34)')

    uGrids.append(MakeUnstructuredGrid(
        vtkCubicLine()))
    titles.append('VTK_CUBIC_LINE (= 35)')

    colors = vtkNamedColors()

    renWin = vtkRenderWindow()
    renWin.SetSize(600, 600)
    renWin.SetWindowName('Isoparametric Cell')

    iRen = vtkRenderWindowInteractor()
    iRen.SetRenderWindow(renWin)

    # Create one sphere for all
    sphere = vtkSphereSource()
    sphere.SetPhiResolution(21)
    sphere.SetThetaResolution(21)
    sphere.SetRadius(.08)

    # Create one text property for all
    textProperty = vtkTextProperty()
    textProperty.SetFontSize(10)
    textProperty.SetJustificationToCentered()

    # Create and link the mappers actors and renderers together.
    for i in range(0, len(uGrids)):
        print('Creating:', titles[i])
        textMappers.append(vtkTextMapper())
        textActors.append(vtkActor2D())

        textMappers[i].GetTextProperty().SetFontSize(10)
        textMappers[i].GetTextProperty().ShadowOn()
        mappers.append(vtkDataSetMapper())
        actors.append(vtkActor())
        renderers.append(vtkRenderer())
        mappers[i].SetInputData(uGrids[i])
        actors[i].SetMapper(mappers[i])
        actors[i].GetProperty().SetColor(colors.GetColor3d('Tomato'))
        actors[i].GetProperty().EdgeVisibilityOn()
        actors[i].GetProperty().SetLineWidth(3)
        actors[i].GetProperty().SetOpacity(.5)
        renderers[i].AddViewProp(actors[i])

        textMappers[i].SetInput(titles[i])
        textActors[i].SetMapper(textMappers[i])
        textActors[i].SetPosition(50, 10)
        renderers[i].AddViewProp(textActors[i])

        # Label the points
        labelMapper = vtkLabeledDataMapper()
        labelMapper.SetInputData(uGrids[i])
        labelActor = vtkActor2D()
        labelActor.SetMapper(labelMapper)
        renderers[i].AddViewProp(labelActor)

        # Glyph the points
        pointMapper = vtkGlyph3DMapper()
        pointMapper.SetInputData(uGrids[i])
        pointMapper.SetSourceConnection(sphere.GetOutputPort())
        pointMapper.ScalingOff()
        pointMapper.ScalarVisibilityOff()

        pointActor = vtkActor()
        pointActor.SetMapper(pointMapper)
        pointActor.GetProperty().SetDiffuseColor(colors.GetColor3d('Banana'))
        pointActor.GetProperty().SetSpecular(.6)
        pointActor.GetProperty().SetSpecularColor(1.0, 1.0, 1.0)
        pointActor.GetProperty().SetSpecularPower(100)
        renderers[i].AddViewProp(pointActor)

        renWin.AddRenderer(renderers[i])

    # Setup the viewports
    xGridDimensions = 4
    yGridDimensions = 4
    rendererSize = 240
    renWin.SetSize(rendererSize * xGridDimensions, rendererSize * yGridDimensions)
    for row in range(0, yGridDimensions):
        for col in range(0, xGridDimensions):
            index = row * xGridDimensions + col

            # (xmin, ymin, xmax, ymax)
            viewport = [float(col) / xGridDimensions,
                        float(yGridDimensions - (row + 1)) / yGridDimensions,
                        float(col + 1) / xGridDimensions,
                        float(yGridDimensions - row) / yGridDimensions]

            if index > (len(actors) - 1):
                # Add a renderer even if there is no actor.
                # This makes the render window background all the same color.
                ren = vtkRenderer()
                ren.SetBackground(colors.GetColor3d('SlateGray'))
                ren.SetViewport(viewport)
                renWin.AddRenderer(ren)
                continue

            renderers[index].SetViewport(viewport)
            renderers[index].SetBackground(colors.GetColor3d('SlateGray'))
            renderers[index].ResetCamera()
            renderers[index].GetActiveCamera().Azimuth(30)
            renderers[index].GetActiveCamera().Elevation(-30)
            renderers[index].ResetCameraClippingRange()

    iRen.Initialize()
    renWin.Render()
    iRen.Start()


# These functions return a vtkUnstructured grid corresponding to the object.

def MakeUnstructuredGrid(aCell):
    pcoords = aCell.GetParametricCoords()
    for i in range(0, aCell.GetNumberOfPoints()):
        aCell.GetPointIds().SetId(i, i)
        aCell.GetPoints().SetPoint(i, (pcoords[3 * i]), (pcoords[3 * i + 1]), (pcoords[3 * i + 2]))

    ug = vtkUnstructuredGrid()
    ug.SetPoints(aCell.GetPoints())
    ug.InsertNextCell(aCell.GetCellType(), aCell.GetPointIds())
    return ug


def MakeQuadraticPolygon():
    quadraticPolygon = vtkQuadraticPolygon()

    quadraticPolygon.GetPointIds().SetNumberOfIds(8)
    quadraticPolygon.GetPointIds().SetId(0, 0)
    quadraticPolygon.GetPointIds().SetId(1, 1)
    quadraticPolygon.GetPointIds().SetId(2, 2)
    quadraticPolygon.GetPointIds().SetId(3, 3)
    quadraticPolygon.GetPointIds().SetId(4, 4)
    quadraticPolygon.GetPointIds().SetId(5, 5)
    quadraticPolygon.GetPointIds().SetId(6, 6)
    quadraticPolygon.GetPointIds().SetId(7, 7)

    quadraticPolygon.GetPoints().SetNumberOfPoints(8)
    quadraticPolygon.GetPoints().SetPoint(0, 0.0, 0.0, 0.0)
    quadraticPolygon.GetPoints().SetPoint(1, 2.0, 0.0, 0.0)
    quadraticPolygon.GetPoints().SetPoint(2, 2.0, 2.0, 0.0)
    quadraticPolygon.GetPoints().SetPoint(3, 0.0, 2.0, 0.0)
    quadraticPolygon.GetPoints().SetPoint(4, 1.0, 0.0, 0.0)
    quadraticPolygon.GetPoints().SetPoint(5, 2.0, 1.0, 0.0)
    quadraticPolygon.GetPoints().SetPoint(6, 1.0, 2.0, 0.0)
    quadraticPolygon.GetPoints().SetPoint(7, 0.0, 1.0, 0.0)
    quadraticPolygon.GetPoints().SetPoint(5, 3.0, 1.0, 0.0)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(quadraticPolygon.GetPoints())
    ug.InsertNextCell(quadraticPolygon.GetCellType(), quadraticPolygon.GetPointIds())
    return ug


if __name__ == '__main__':
    main()
