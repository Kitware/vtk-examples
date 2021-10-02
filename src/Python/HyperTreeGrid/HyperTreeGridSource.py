#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersGeneral import vtkShrinkFilter
from vtkmodules.vtkFiltersHyperTree import vtkHyperTreeGridToUnstructuredGrid
from vtkmodules.vtkFiltersSources import vtkHyperTreeGridSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()
    # Create hyper tree grid source

    descriptor = 'RRR .R. .RR ..R ..R .R.|R.......................... ' \
                 '........................... ........................... ' \
                 '.............R............. ....RR.RR........R......... ' \
                 '.....RRRR.....R.RR......... ........................... ' \
                 '........................... ' \
                 '...........................|........................... ' \
                 '........................... ........................... ' \
                 '...RR.RR.......RR.......... ........................... ' \
                 'RR......................... ........................... ' \
                 '........................... ........................... ' \
                 '........................... ........................... ' \
                 '........................... ........................... ' \
                 '............RRR............|........................... ' \
                 '........................... .......RR.................. ' \
                 '........................... ........................... ' \
                 '........................... ........................... ' \
                 '........................... ........................... ' \
                 '........................... ' \
                 '...........................|........................... ' \
                 '...........................'

    source = vtkHyperTreeGridSource()
    source.SetMaximumLevel(6)
    source.SetDimensions(4, 4, 3)  # GridCell 3, 3, 2
    source.SetGridScale(1.5, 1.0, 0.7)
    source.SetBranchFactor(4)
    source.SetDescriptor(descriptor)
    source.Update()

    # Hyper tree grid to unstructured grid filter
    htg2ug = vtkHyperTreeGridToUnstructuredGrid()
    htg2ug.SetInputConnection(source.GetOutputPort())
    htg2ug.Update()

    shrink = vtkShrinkFilter()
    shrink.SetInputConnection(htg2ug.GetOutputPort())
    shrink.SetShrinkFactor(.8)

    mapper = vtkDataSetMapper()
    mapper.SetInputConnection(shrink.GetOutputPort())
    mapper.ScalarVisibilityOff()

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetDiffuseColor(colors.GetColor3d('Burlywood'))

    # Create the RenderWindow, Renderer and Interactor
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    renderer.SetBackground(colors.GetColor3d('SlateGray'))
    renderer.AddActor(actor)
    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(150)
    renderer.GetActiveCamera().Elevation(30)
    renderer.ResetCameraClippingRange()

    renderWindow.SetSize(640, 480)
    renderWindow.Render()
    renderWindow.SetWindowName('HyperTreeGridSource')

    interactor.Start()


if __name__ == '__main__':
    main()
