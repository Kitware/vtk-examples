#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkDataObject
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkHardwareSelector,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)

# Callback for when selection is changed

# This is global - fix later.
ren1 = vtkRenderer()


def selectionCallback(caller, eventId):
    hsel = vtkHardwareSelector()
    hsel.SetFieldAssociation(vtkDataObject.FIELD_ASSOCIATION_CELLS)
    hsel.SetRenderer(ren1)

    x, y = caller.GetRenderWindow().GetSize()

    # Create a small area around clicked point for selector area
    hsel.SetArea(0, 0, x, y)
    res = hsel.Select()

    numNodes = res.GetNumberOfNodes()
    if (numNodes < 1):
        print("No visible cells")
    else:
        sel_node = res.GetNode(0)
        print('Visible cell IDs: ', VN.vtk_to_numpy(sel_node.GetSelectionList()).tolist())


def main():
    colors = vtkNamedColors()

    sphere = vtkSphereSource()
    sphere.SetCenter(0, 0, 0)
    sphere.SetRadius(5.0)

    sphereMapper = vtkPolyDataMapper()
    sphereMapper.SetInputConnection(sphere.GetOutputPort())

    sphereActor = vtkActor()
    sphereActor.SetMapper(sphereMapper)
    sphereActor.GetProperty().SetColor(colors.GetColor3d('Bisque'))

    ren1.AddActor(sphereActor)
    ren1.GetActiveCamera().ParallelProjectionOn()
    ren1.SetBackground(colors.GetColor3d('Navy'))

    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren1)
    renWin.SetSize(300, 300)
    renWin.SetWindowName('HardwareSelector')

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)
    iren.AddObserver("UserEvent", selectionCallback)

    style = vtkInteractorStyleTrackballCamera()
    iren.SetInteractorStyle(style)
    renWin.GetInteractor().SetInteractorStyle(style)

    ren1.ResetCamera()
    renWin.Render()

    iren.Initialize()
    iren.Start()


if __name__ == '__main__':
    main()
