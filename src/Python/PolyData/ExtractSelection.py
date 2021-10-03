#!/usr/bin/env python

'''
converted from:
 - http://www.org/Wiki/VTK/Examples/Python/PolyData/ExtractSelectionCells
'''

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkIdTypeArray
from vtkmodules.vtkCommonDataModel import (
    vtkSelection,
    vtkSelectionNode,
    vtkUnstructuredGrid
)
from vtkmodules.vtkFiltersExtraction import vtkExtractSelection
from vtkmodules.vtkFiltersSources import vtkPointSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # colors.SetColor('leftBkg', [0.6, 0.5, 0.4, 1.0])
    # colors.SetColor('centreBkg', [0.3, 0.1, 0.4, 1.0])
    # colors.SetColor('rightBkg', [0.4, 0.5, 0.6, 1.0])

    pointSource = vtkPointSource()
    pointSource.SetNumberOfPoints(50)
    pointSource.Update()

    print('There are %s input points\n' % pointSource.GetOutput().GetNumberOfPoints())

    ids = vtkIdTypeArray()
    ids.SetNumberOfComponents(1)

    # Set values
    i = 10
    while i < 20:
        ids.InsertNextValue(i)
        i += 1

    selectionNode = vtkSelectionNode()
    selectionNode.SetFieldType(1)  # POINT
    #  CELL_DATA = 0
    #  POINT_DATA = 1
    #  FIELD_DATA = 2
    #  VERTEX_DATA = 3
    #  EDGE_DATA = 4

    selectionNode.SetContentType(4)  # INDICES
    # SELECTIONS = 0
    # GLOBALIDS = 1
    # PEDIGREEIDS = 2
    # VALUES = 3
    # INDICES = 4
    # FRUSTUM = 5
    # LOCATIONS = 6
    # THRESHOLDS = 7
    # BLOCKS = 8
    selectionNode.SetSelectionList(ids)

    selection = vtkSelection()
    selection.AddNode(selectionNode)

    extractSelection = vtkExtractSelection()

    extractSelection.SetInputConnection(0, pointSource.GetOutputPort())
    extractSelection.SetInputData(1, selection)
    extractSelection.Update()

    # In selection
    selected = vtkUnstructuredGrid()
    selected.ShallowCopy(extractSelection.GetOutput())

    print('There are %s points in the selection' % selected.GetNumberOfPoints())
    print('There are %s cells in the selection' % selected.GetNumberOfCells())

    # Get points that are NOT in the selection
    # invert the selection
    selectionNode.GetProperties().Set(vtkSelectionNode.INVERSE(), 1)
    extractSelection.Update()

    notSelected = vtkUnstructuredGrid()
    notSelected.ShallowCopy(extractSelection.GetOutput())

    print('There are %s points NOT in the selection' % notSelected.GetNumberOfPoints())
    print('There are %s cells NOT in the selection' % notSelected.GetNumberOfCells())

    inputMapper = vtkDataSetMapper()
    inputMapper.SetInputConnection(pointSource.GetOutputPort())
    inputActor = vtkActor()
    inputActor.SetMapper(inputMapper)
    inputActor.GetProperty().SetColor(colors.GetColor3d('MidnightBlue'))
    inputActor.GetProperty().SetPointSize(5)

    selectedMapper = vtkDataSetMapper()
    selectedMapper.SetInputData(selected)
    selectedActor = vtkActor()
    selectedActor.SetMapper(selectedMapper)
    selectedActor.GetProperty().SetColor(colors.GetColor3d('MidnightBlue'))
    selectedActor.GetProperty().SetPointSize(5)

    notSelectedMapper = vtkDataSetMapper()
    notSelectedMapper.SetInputData(notSelected)
    notSelectedActor = vtkActor()
    notSelectedActor.SetMapper(notSelectedMapper)
    notSelectedActor.GetProperty().SetColor(colors.GetColor3d('MidnightBlue'))
    notSelectedActor.GetProperty().SetPointSize(5)

    # There will be one render window
    renderWindow = vtkRenderWindow()
    renderWindow.SetSize(900, 300)
    renderWindow.SetWindowName('ExtractSelection')

    # And one interactor
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    # Define viewport ranges
    # (xmin, ymin, xmax, ymax)
    leftViewport = [0.0, 0.0, 0.33, 1.0]
    centerViewport = [0.33, 0.0, 0.66, 1.0]
    rightViewport = [0.66, 0.0, 1.0, 1.0]

    # Create a camera for all renderers
    camera = vtkCamera()

    # Setup the renderers
    leftRenderer = vtkRenderer()
    renderWindow.AddRenderer(leftRenderer)
    leftRenderer.SetViewport(leftViewport)
    leftRenderer.SetBackground(colors.GetColor3d('BurlyWood'))
    leftRenderer.SetActiveCamera(camera)

    centerRenderer = vtkRenderer()
    renderWindow.AddRenderer(centerRenderer)
    centerRenderer.SetViewport(centerViewport)
    centerRenderer.SetBackground(colors.GetColor3d('orchid_dark'))
    centerRenderer.SetActiveCamera(camera)

    rightRenderer = vtkRenderer()
    renderWindow.AddRenderer(rightRenderer)
    rightRenderer.SetViewport(rightViewport)
    rightRenderer.SetBackground(colors.GetColor3d('CornflowerBlue'))
    rightRenderer.SetActiveCamera(camera)

    leftRenderer.AddActor(inputActor)
    centerRenderer.AddActor(selectedActor)
    rightRenderer.AddActor(notSelectedActor)

    leftRenderer.ResetCamera()

    renderWindow.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
