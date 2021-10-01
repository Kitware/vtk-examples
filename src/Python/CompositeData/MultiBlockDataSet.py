#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkMultiBlockDataSet
from vtkmodules.vtkFiltersExtraction import vtkExtractEdges
from vtkmodules.vtkFiltersGeometry import vtkCompositeDataGeometryFilter
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # PART 1 Make some Data.
    # Make a tree.
    root = vtkMultiBlockDataSet()

    branch = vtkMultiBlockDataSet()
    root.SetBlock(0, branch)

    # Make some leaves.
    leaf1 = vtkSphereSource()
    leaf1.SetCenter(0, 0, 0)
    leaf1.Update()
    branch.SetBlock(0, leaf1.GetOutput())

    leaf2 = vtkSphereSource()
    leaf2.SetCenter(1.75, 2.5, 0)
    leaf2.SetRadius(1.5)
    leaf2.Update()
    branch.SetBlock(1, leaf2.GetOutput())

    leaf3 = vtkSphereSource()
    leaf3.SetCenter(4, 0, 0)
    leaf3.SetRadius(2)
    leaf3.Update()
    root.SetBlock(1, leaf3.GetOutput())

    # PART 2 Do something with the data
    # a non composite aware filter, the pipeline will iterate
    edges = vtkExtractEdges()
    edges.SetInputData(root)

    # PART 3 Show the data
    # also demonstrate a composite aware filter
    # this filter aggregates all blocks into one polydata
    # this is handy for display, although fairly limited
    # see vtkCompositePolyDataMapper2 for something better
    polydata = vtkCompositeDataGeometryFilter()
    polydata.SetInputConnection(edges.GetOutputPort())

    # Create the Renderer, RenderWindow, and RenderWindowInteractor.
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(0, polydata.GetOutputPort(0))
    actor = vtkActor()
    actor.GetProperty().SetColor(colors.GetColor3d('Yellow'))
    actor.GetProperty().SetLineWidth(2)
    actor.SetMapper(mapper)

    # Enable user interface interactor.
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('CornflowerBlue'))
    renderWindow.SetWindowName('MultiBlockDataSet')
    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
