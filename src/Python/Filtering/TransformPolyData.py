#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersGeneral import vtkTransformPolyDataFilter
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

    # Create the polydata geometry
    sphereSource = vtkSphereSource()
    sphereSource.Update()

    # Set up the actor to display the untransformed polydata
    originalMapper = vtkPolyDataMapper()
    originalMapper.SetInputConnection(sphereSource.GetOutputPort())

    originalActor = vtkActor()
    originalActor.SetMapper(originalMapper)
    originalActor.GetProperty().SetColor(colors.GetColor3d('Blue'))

    # Set up the transform filter
    translation = vtkTransform()
    translation.Translate(1.0, 2.0, 3.0)

    transformFilter = vtkTransformPolyDataFilter()
    transformFilter.SetInputConnection(sphereSource.GetOutputPort())
    transformFilter.SetTransform(translation)
    transformFilter.Update()

    # Set up the actor to display the transformed polydata
    transformedMapper = vtkPolyDataMapper()
    transformedMapper.SetInputConnection(transformFilter.GetOutputPort())

    transformedActor = vtkActor()
    transformedActor.SetMapper(transformedMapper)
    transformedActor.GetProperty().SetColor(colors.GetColor3d('Red'))

    # Set up the rest of the visualization pipeline
    renderer = vtkRenderer()
    renderer.AddActor(originalActor)
    renderer.AddActor(transformedActor)
    renderer.SetBackground(colors.GetColor3d('Green'))

    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderWindow.SetWindowName('TransformPolyData')
    renderWindow.Render()

    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
