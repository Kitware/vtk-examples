#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import (
    vtkAppendFilter,
    vtkConnectivityFilter,
    vtkDelaunay3D
)
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    sphereSource1 = vtkSphereSource()
    sphereSource1.Update()

    delaunay1 = vtkDelaunay3D()
    delaunay1.SetInputConnection(sphereSource1.GetOutputPort())
    delaunay1.Update()

    sphereSource2 = vtkSphereSource()
    sphereSource2.SetCenter(5, 0, 0)
    sphereSource2.Update()

    delaunay2 = vtkDelaunay3D()
    delaunay2.SetInputConnection(sphereSource2.GetOutputPort())
    delaunay2.Update()

    appendFilter = vtkAppendFilter()
    appendFilter.AddInputConnection(delaunay1.GetOutputPort())
    appendFilter.AddInputConnection(delaunay2.GetOutputPort())
    appendFilter.Update()

    connectivityFilter = vtkConnectivityFilter()
    connectivityFilter.SetInputConnection(appendFilter.GetOutputPort())
    connectivityFilter.SetExtractionModeToAllRegions()
    connectivityFilter.ColorRegionsOn()
    connectivityFilter.Update()

    # Visualize
    mapper = vtkDataSetMapper()
    mapper.SetInputConnection(connectivityFilter.GetOutputPort())
    mapper.Update()

    actor = vtkActor()
    actor.SetMapper(mapper)

    renderer = vtkRenderer()
    renderer.AddActor(actor)

    # renWindow = vtkRenderWindow()
    # renWindow.AddRenderer(renderer)
    # iren = vtkRenderWindowInteractor()
    # iren.SetRenderWindow(renWindow)
    # iren.Initialize()
    # iren.Start()
    renWindow = vtkRenderWindow()
    renWindow.AddRenderer(renderer)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWindow)

    iren.Initialize()
    renWindow.Render()
    renWindow.SetWindowName('ConnectivityFilter')
    renderer.SetBackground(colors.GetColor3d('deep_ochre'))
    renderer.GetActiveCamera().Zoom(0.9)
    renWindow.Render()
    iren.Start()


if __name__ == '__main__':
    main()
