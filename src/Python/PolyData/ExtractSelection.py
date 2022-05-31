#!/usr/bin/env python3

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


def main(argv):
    colors = vtkNamedColors()

    point_source = vtkPointSource()

    point_source.SetNumberOfPoints(50)
    point_source.Update()

    print('There are', point_source.GetOutput().GetNumberOfPoints(), 'input points.')

    ids = vtkIdTypeArray()
    ids.SetNumberOfComponents(1)

    # Set values.
    for i in range(10, 20):
        ids.InsertNextValue(i)

    selection_node = vtkSelectionNode()
    selection_node.SetFieldType(vtkSelectionNode.POINT)
    selection_node.SetContentType(vtkSelectionNode.INDICES)
    selection_node.SetSelectionList(ids)

    selection = vtkSelection()
    selection.AddNode(selection_node)

    extract_selection = vtkExtractSelection()
    extract_selection.SetInputConnection(0, point_source.GetOutputPort())
    extract_selection.SetInputData(1, selection)
    extract_selection.Update()

    # In selection.
    selected = vtkUnstructuredGrid()
    selected.ShallowCopy(extract_selection.GetOutput())

    print('There are', selected.GetNumberOfPoints(), 'points and', selected.GetNumberOfCells(),
          'cells in the selection.')

    # Get points that are NOT in the selection.
    selection_node.GetProperties().Set(vtkSelectionNode().INVERSE(), 1)  # invert the selection.
    extract_selection.Update()

    not_selected = vtkUnstructuredGrid()
    not_selected.ShallowCopy(extract_selection.GetOutput())

    print('There are', not_selected.GetNumberOfPoints(), 'points and', not_selected.GetNumberOfCells(),
          'cells NOT in the selection.')

    input_mapper = vtkDataSetMapper()
    input_mapper.SetInputConnection(point_source.GetOutputPort())
    input_actor = vtkActor()
    input_actor.SetMapper(input_mapper)
    input_actor.GetProperty().SetColor(colors.GetColor3d("MidnightBlue"))
    input_actor.GetProperty().SetPointSize(5)

    selected_mapper = vtkDataSetMapper()
    selected_mapper.SetInputData(selected)

    selected_actor = vtkActor()
    selected_actor.SetMapper(selected_mapper)
    selected_actor.GetProperty().SetColor(colors.GetColor3d("MidnightBlue"))
    selected_actor.GetProperty().SetPointSize(5)

    not_selected_mapper = vtkDataSetMapper()
    not_selected_mapper.SetInputData(not_selected)

    not_selected_actor = vtkActor()
    not_selected_actor.SetMapper(not_selected_mapper)
    not_selected_actor.GetProperty().SetColor(colors.GetColor3d("MidnightBlue"))
    not_selected_actor.GetProperty().SetPointSize(5)

    # There will be one render window.
    render_window = vtkRenderWindow()
    render_window.SetSize(900, 300)
    render_window.SetWindowName("ExtractSelectedIds")

    # And one interactor.
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(render_window)

    # Define viewport ranges.
    # (xmin, ymin, xmax, ymax)
    left_viewport = [0.0, 0.0, 0.33, 1.0]
    center_viewport = [0.33, 0.0, 0.66, 1.0]
    right_viewport = [0.66, 0.0, 1.0, 1.0]

    # Create a camera for all renderers.
    camera = vtkCamera()

    # Setup the renderers
    left_renderer = vtkRenderer()
    render_window.AddRenderer(left_renderer)
    left_renderer.SetViewport(left_viewport)
    left_renderer.SetBackground(colors.GetColor3d("BurlyWood"))
    left_renderer.SetActiveCamera(camera)

    center_renderer = vtkRenderer()
    render_window.AddRenderer(center_renderer)
    center_renderer.SetViewport(center_viewport)
    center_renderer.SetBackground(colors.GetColor3d("orchid_dark"))
    center_renderer.SetActiveCamera(camera)

    right_renderer = vtkRenderer()
    render_window.AddRenderer(right_renderer)
    right_renderer.SetViewport(right_viewport)
    right_renderer.SetBackground(colors.GetColor3d("CornflowerBlue"))
    right_renderer.SetActiveCamera(camera)

    left_renderer.AddActor(input_actor)
    center_renderer.AddActor(selected_actor)
    right_renderer.AddActor(not_selected_actor)

    left_renderer.ResetCamera()

    render_window.Render()
    interactor.Start()


if __name__ == '__main__':
    import sys

    main(sys.argv)
