#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkGlyph3D
from vtkmodules.vtkFiltersSources import (
    vtkConeSource,
    vtkSphereSource
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    renderer = vtkRenderer()

    renWin = vtkRenderWindow()
    renWin.AddRenderer(renderer)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    sphere = vtkSphereSource()
    sphere.SetThetaResolution(8)
    sphere.SetPhiResolution(8)

    sphereMapper = vtkPolyDataMapper()
    sphereMapper.SetInputConnection(sphere.GetOutputPort())

    sphereActor = vtkActor()
    sphereActor.SetMapper(sphereMapper)
    sphereActor.GetProperty().SetColor(colors.GetColor3d('Silver'))

    cone = vtkConeSource()
    cone.SetResolution(6)

    glyph = vtkGlyph3D()
    glyph.SetInputConnection(sphere.GetOutputPort())
    glyph.SetSourceConnection(cone.GetOutputPort())
    glyph.SetVectorModeToUseNormal()
    glyph.SetScaleModeToScaleByVector()
    glyph.SetScaleFactor(0.25)

    spikeMapper = vtkPolyDataMapper()
    spikeMapper.SetInputConnection(glyph.GetOutputPort())

    spikeActor = vtkActor()
    spikeActor.SetMapper(spikeMapper)
    spikeActor.GetProperty().SetColor(colors.GetColor3d('Silver'))

    renderer.AddActor(sphereActor)
    renderer.AddActor(spikeActor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))
    renWin.SetSize(640, 480)
    renWin.SetWindowName('Mace')

    # Interact with the data.
    renWin.Render()

    iren.Start()


if __name__ == '__main__':
    main()
