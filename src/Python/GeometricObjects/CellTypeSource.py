# !/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingFreeType
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import (
    vtkColorSeries,
    vtkNamedColors
)
from vtkmodules.vtkCommonCore import (
    vtkIntArray,
    vtkLookupTable,
    vtkMinimalStandardRandomSequence,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import (
    VTK_CUBIC_LINE,
    VTK_HEXAHEDRON,
    VTK_LINE,
    VTK_PYRAMID,
    VTK_QUAD,
    VTK_QUADRATIC_EDGE,
    VTK_QUADRATIC_HEXAHEDRON,
    VTK_QUADRATIC_PYRAMID,
    VTK_QUADRATIC_QUAD,
    VTK_QUADRATIC_TETRA,
    VTK_QUADRATIC_TRIANGLE,
    VTK_QUADRATIC_WEDGE,
    VTK_TETRA,
    VTK_TRIANGLE,
    VTK_WEDGE,
    vtkCellTypes
)
from vtkmodules.vtkFiltersGeneral import (
    vtkShrinkFilter,
    vtkTessellatorFilter
)
from vtkmodules.vtkFiltersSources import vtkCellTypeSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkActor2D,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTextMapper,
    vtkTextProperty
)


def main():
    cellName = get_program_parameters()

    # Store the cell class names in a dictionary.
    cellMap = dict()
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_LINE)] = VTK_LINE
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_QUADRATIC_EDGE)] = VTK_QUADRATIC_EDGE
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_CUBIC_LINE)] = VTK_CUBIC_LINE

    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_TRIANGLE)] = VTK_TRIANGLE
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_QUADRATIC_TRIANGLE)] = VTK_QUADRATIC_TRIANGLE
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_QUAD)] = VTK_QUAD
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_QUADRATIC_QUAD)] = VTK_QUADRATIC_QUAD

    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_TETRA)] = VTK_TETRA
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_HEXAHEDRON)] = VTK_HEXAHEDRON
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_WEDGE)] = VTK_WEDGE
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_PYRAMID)] = VTK_PYRAMID
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_QUADRATIC_WEDGE)] = VTK_QUADRATIC_WEDGE
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_QUADRATIC_PYRAMID)] = VTK_QUADRATIC_PYRAMID
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_QUADRATIC_HEXAHEDRON)] = VTK_QUADRATIC_HEXAHEDRON
    cellMap[vtkCellTypes.GetClassNameFromTypeId(VTK_QUADRATIC_TETRA)] = VTK_QUADRATIC_TETRA

    if cellName not in cellMap:
        print('Cell type ', cellName, ' is not supported.')
        return
    source = vtkCellTypeSource()
    source.SetCellType(cellMap[cellName])
    source.Update()
    print('Cell: ', cellName)

    originalPoints = source.GetOutput().GetPoints()
    points = vtkPoints()
    points.SetNumberOfPoints(source.GetOutput().GetNumberOfPoints())
    rng = vtkMinimalStandardRandomSequence()
    rng.SetSeed(5070)  # for testing
    for i in range(0, points.GetNumberOfPoints()):
        perturbation = [0.0] * 3
        for j in range(0, 3):
            rng.Next()
            perturbation[j] = rng.GetRangeValue(-0.1, 0.1)
        currentPoint = [0.0] * 3
        originalPoints.GetPoint(i, currentPoint)
        points.SetPoint(i, currentPoint[0] + perturbation[0],
                        currentPoint[1] + perturbation[1],
                        currentPoint[2] + perturbation[2])
    source.GetOutput().SetPoints(points)

    numCells = source.GetOutput().GetNumberOfCells()
    print('Number of cells: ', numCells)
    idArray = vtkIntArray()
    idArray.SetNumberOfTuples(numCells)
    for i in range(0, numCells):
        idArray.InsertTuple1(i, i + 1)
    idArray.SetName('Ids')
    source.GetOutput().GetCellData().AddArray(idArray)
    source.GetOutput().GetCellData().SetActiveScalars('Ids')

    shrink = vtkShrinkFilter()
    shrink.SetInputConnection(source.GetOutputPort())
    shrink.SetShrinkFactor(.8)

    tessellate = vtkTessellatorFilter()
    tessellate.SetInputConnection(shrink.GetOutputPort())
    tessellate.SetMaximumNumberOfSubdivisions(3)

    # Create a lookup table to map cell data to colors.
    lut = vtkLookupTable()

    colorSeries = vtkColorSeries()
    seriesEnum = colorSeries.BREWER_QUALITATIVE_SET3
    colorSeries.SetColorScheme(seriesEnum)
    colorSeries.BuildLookupTable(lut, colorSeries.ORDINAL)

    # Fill in a few known colors, the rest will be generated if needed.
    colors = vtkNamedColors()

    # Create a mapper and actor.
    mapper = vtkDataSetMapper()
    mapper.SetInputConnection(source.GetOutputPort())
    mapper.SetInputConnection(shrink.GetOutputPort())
    mapper.SetScalarRange(0, numCells + 1)
    mapper.SetLookupTable(lut)
    mapper.SetScalarModeToUseCellData()
    mapper.SetResolveCoincidentTopologyToPolygonOffset()
    if (source.GetCellType() == VTK_QUADRATIC_PYRAMID or
            source.GetCellType() == VTK_QUADRATIC_WEDGE):
        mapper.SetInputConnection(shrink.GetOutputPort())
    else:
        mapper.SetInputConnection(tessellate.GetOutputPort())
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().EdgeVisibilityOn()
    #  actor.GetProperty().SetLineWidth(3)

    textProperty = vtkTextProperty()
    textProperty.SetFontSize(20)
    textProperty.SetJustificationToCentered()
    textProperty.SetColor(colors.GetColor3d('Lamp_Black'))

    textMapper = vtkTextMapper()
    textMapper.SetInput(cellName)
    textMapper.SetTextProperty(textProperty)

    textActor = vtkActor2D()
    textActor.SetMapper(textMapper)
    textActor.SetPosition(320, 20)

    # Create a renderer, render window, and interactor.
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName('CellTypeSource')
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the actors to the scene.
    renderer.AddViewProp(textActor)
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('Silver'))

    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(30)
    renderer.GetActiveCamera().Elevation(30)
    renderer.ResetCameraClippingRange()

    # Render and interact.
    renderWindow.SetSize(640, 480)
    renderWindow.Render()
    renderWindowInteractor.Start()


def get_program_parameters():
    import argparse
    description = 'Cell Type Source.'
    epilogue = '''
    You can supply an optional argument consisting of a vtkCell name e.g: vtkTriangle.
    The default is vtkTetra.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('cell_name', nargs='?', const='vtkTetra', default='vtkTetra', type=str, help='The cell name.')
    args = parser.parse_args()
    return args.cell_name


if __name__ == '__main__':
    main()
