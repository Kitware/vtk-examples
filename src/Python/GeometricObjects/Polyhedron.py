#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkIdList,
    vtkPoints
)
from vtkmodules.vtkCommonDataModel import (
    VTK_POLYHEDRON,
    vtkUnstructuredGrid
)
from vtkmodules.vtkIOXML import vtkXMLUnstructuredGridWriter
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # create polyhedron (cube)
    # The point Ids are: [0, 1, 2, 3, 4, 5, 6, 7]

    points = vtkPoints()
    points.InsertNextPoint(-1.0, -1.0, -1.0)
    points.InsertNextPoint(1.0, -1.0, -1.0)
    points.InsertNextPoint(1.0, 1.0, -1.0)
    points.InsertNextPoint(-1.0, 1.0, -1.0)
    points.InsertNextPoint(-1.0, -1.0, 1.0)
    points.InsertNextPoint(1.0, -1.0, 1.0)
    points.InsertNextPoint(1.0, 1.0, 1.0)
    points.InsertNextPoint(-1.0, 1.0, 1.0)

    # These are the point ids corresponding to each face.
    faces = [[0, 3, 2, 1], [0, 4, 7, 3], [4, 5, 6, 7], [5, 1, 2, 6], [0, 1, 5, 4], [2, 3, 7, 6]]
    faceId = vtkIdList()
    faceId.InsertNextId(6)  # Six faces make up the cell.
    for face in faces:
        faceId.InsertNextId(len(face))  # The number of points in the face.
        [faceId.InsertNextId(i) for i in face]

    ugrid = vtkUnstructuredGrid()
    ugrid.SetPoints(points)
    ugrid.InsertNextCell(VTK_POLYHEDRON, faceId)

    # Here we write out the cube.
    writer = vtkXMLUnstructuredGridWriter()
    writer.SetInputData(ugrid)
    writer.SetFileName('polyhedron.vtu')
    writer.SetDataModeToAscii()
    writer.Update()

    # Create a mapper and actor
    mapper = vtkDataSetMapper()
    mapper.SetInputData(ugrid)

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(
        colors.GetColor3d('Silver'))

    # Visualize
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName('Polyhedron')
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('Salmon'))
    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(30)
    renderer.GetActiveCamera().Elevation(30)
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
