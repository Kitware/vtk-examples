#!/usr/bin/env python

import vtkmodules.all as vtk


# Catch mouse events
class MouseInteractorStyle(vtk.vtkInteractorStyleTrackballCamera):
    def __init__(self, data):
        self.AddObserver('LeftButtonPressEvent', self.left_button_press_event)
        self.data = data
        self.selected_mapper = vtk.vtkDataSetMapper()
        self.selected_actor = vtk.vtkActor()

    def left_button_press_event(self, obj, event):
        colors = vtk.vtkNamedColors()

        # Get the location of the click (in window coordinates)
        pos = self.GetInteractor().GetEventPosition()

        picker = vtk.vtkCellPicker()
        picker.SetTolerance(0.0005)

        # Pick from this location.
        picker.Pick(pos[0], pos[1], 0, self.GetDefaultRenderer())

        world_position = picker.GetPickPosition()
        print(f'Cell id is: {picker.GetCellId()}')

        if picker.GetCellId() != -1:
            print(f'Pick position is: ({world_position[0]:.6g}, {world_position[1]:.6g}, {world_position[2]:.6g})')

            ids = vtk.vtkIdTypeArray()
            ids.SetNumberOfComponents(1)
            ids.InsertNextValue(picker.GetCellId())

            selection_node = vtk.vtkSelectionNode()
            selection_node.SetFieldType(vtk.vtkSelectionNode.CELL)
            selection_node.SetContentType(vtk.vtkSelectionNode.INDICES)
            selection_node.SetSelectionList(ids)

            selection = vtk.vtkSelection()
            selection.AddNode(selection_node)

            extract_selection = vtk.vtkExtractSelection()
            extract_selection.SetInputData(0, self.data)
            extract_selection.SetInputData(1, selection)
            extract_selection.Update()

            # In selection
            selected = vtk.vtkUnstructuredGrid()
            selected.ShallowCopy(extract_selection.GetOutput())

            print(f'Number of points in the selection: {selected.GetNumberOfPoints()}')
            print(f'Number of cells in the selection : {selected.GetNumberOfCells()}')

            self.selected_mapper.SetInputData(selected)
            self.selected_actor.SetMapper(self.selected_mapper)
            self.selected_actor.GetProperty().EdgeVisibilityOn()
            self.selected_actor.GetProperty().SetColor(colors.GetColor3d('Tomato'))

            self.selected_actor.GetProperty().SetLineWidth(3)

            self.GetInteractor().GetRenderWindow().GetRenderers().GetFirstRenderer().AddActor(self.selected_actor)

        # Forward events
        self.OnLeftButtonDown()


def main(argv):
    colors = vtk.vtkNamedColors()

    plane_source = vtk.vtkPlaneSource()
    plane_source.Update()

    triangle_filter = vtk.vtkTriangleFilter()
    triangle_filter.SetInputConnection(plane_source.GetOutputPort())
    triangle_filter.Update()

    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(triangle_filter.GetOutputPort())

    actor = vtk.vtkActor()
    actor.GetProperty().SetColor(colors.GetColor3d('SeaGreen'))
    actor.SetMapper(mapper)

    renderer = vtk.vtkRenderer()
    ren_win = vtk.vtkRenderWindow()
    ren_win.AddRenderer(renderer)
    ren_win.SetWindowName('CellPicking')
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    renderer.AddActor(actor)
    # renderer.ResetCamera()
    renderer.SetBackground(colors.GetColor3d('PaleTurquoise'))

    # Add the custom style.
    style = MouseInteractorStyle(triangle_filter.GetOutput())
    style.SetDefaultRenderer(renderer)
    iren.SetInteractorStyle(style)

    ren_win.Render()
    iren.Initialize()
    iren.Start()


if __name__ == '__main__':
    import sys

    main(sys.argv)
