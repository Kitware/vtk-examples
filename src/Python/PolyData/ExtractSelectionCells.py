#!/usr/bin/env python

'''
converted from:
 - http://www.org/Wiki/VTK/Examples/Cxx/PolyData/ExtractSelectionCells
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
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkDataSetMapper,
    vtkProperty,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # colors.SetColor('leftBkg', [0.6, 0.5, 0.4, 1.0])
    # colors.SetColor('centreBkg', [0.3, 0.1, 0.4, 1.0])
    # colors.SetColor('rightBkg', [0.4, 0.5, 0.6, 1.0])

    sphereSource = vtkSphereSource()
    sphereSource.Update()

    print('There are %s input points' % sphereSource.GetOutput().GetNumberOfPoints())
    print('There are %s input cells' % sphereSource.GetOutput().GetNumberOfCells())

    ids = vtkIdTypeArray()
    ids.SetNumberOfComponents(1)

    # Specify that we want to extract cells 10 through 19
    i = 10
    while i < 20:
        ids.InsertNextValue(i)
        i += 1

    selectionNode = vtkSelectionNode()
    selectionNode.SetFieldType(vtkSelectionNode.CELL)
    selectionNode.SetContentType(vtkSelectionNode.INDICES)
    selectionNode.SetSelectionList(ids)

    selection = vtkSelection()
    selection.AddNode(selectionNode)

    extractSelection = vtkExtractSelection()
    extractSelection.SetInputConnection(0, sphereSource.GetOutputPort())
    extractSelection.SetInputData(1, selection)
    extractSelection.Update()

    # In selection
    selected = vtkUnstructuredGrid()
    selected.ShallowCopy(extractSelection.GetOutput())

    print('There are %s points in the selection' % selected.GetNumberOfPoints())
    print('There are %s cells in the selection' % selected.GetNumberOfCells())

    # Get points that are NOT in the selection
    selectionNode.GetProperties().Set(vtkSelectionNode.INVERSE(), 1)  # invert the selection
    extractSelection.Update()

    notSelected = vtkUnstructuredGrid()
    notSelected.ShallowCopy(extractSelection.GetOutput())

    print('There are %s points NOT in the selection' % notSelected.GetNumberOfPoints())
    print('There are %s cells NOT in the selection' % notSelected.GetNumberOfCells())

    backfaces = vtkProperty()
    backfaces.SetColor(colors.GetColor3d('Gold'))

    inputMapper = vtkDataSetMapper()
    inputMapper.SetInputConnection(sphereSource.GetOutputPort())
    inputActor = vtkActor()
    inputActor.SetMapper(inputMapper)
    inputActor.SetBackfaceProperty(backfaces)
    inputActor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    selectedMapper = vtkDataSetMapper()
    selectedMapper.SetInputData(selected)

    selectedActor = vtkActor()
    selectedActor.SetMapper(selectedMapper)
    selectedActor.SetBackfaceProperty(backfaces)
    selectedActor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    notSelectedMapper = vtkDataSetMapper()
    notSelectedMapper.SetInputData(notSelected)

    notSelectedActor = vtkActor()
    notSelectedActor.SetMapper(notSelectedMapper)
    notSelectedActor.SetBackfaceProperty(backfaces)
    notSelectedActor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    # There will be one render window
    renderWindow = vtkRenderWindow()
    renderWindow.SetSize(900, 300)
    renderWindow.SetWindowName('ExtractSelectionCells')

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
