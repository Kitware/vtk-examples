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

    # Set the background color.
    colors.SetColor('Bkg', [26, 51, 102, 255])

    # Create the rendering objects.
    ren1 = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren1)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Create the pipeline, ball and spikes.
    sphere = vtkSphereSource()
    sphere.SetPhiResolution(7)
    sphere.SetThetaResolution(7)
    sphereMapper = vtkPolyDataMapper()
    sphereMapper.SetInputConnection(sphere.GetOutputPort())
    sphereActor = vtkActor()
    sphereActor.SetMapper(sphereMapper)
    sphereActor2 = vtkActor()
    sphereActor2.SetMapper(sphereMapper)

    cone = vtkConeSource()
    cone.SetResolution(5)
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
    spikeActor2 = vtkActor()
    spikeActor2.SetMapper(spikeMapper)

    spikeActor.SetPosition(0, 0.7, 0)
    sphereActor.SetPosition(0, 0.7, 0)
    spikeActor2.SetPosition(0, -1.0, -10)
    sphereActor2.SetPosition(0, -1.0, -10)
    spikeActor2.SetScale(1.5, 1.5, 1.5)
    sphereActor2.SetScale(1.5, 1.5, 1.5)

    ren1.AddActor(sphereActor)
    ren1.AddActor(spikeActor)
    ren1.AddActor(sphereActor2)
    ren1.AddActor(spikeActor2)
    ren1.SetBackground(colors.GetColor3d('Bkg'))
    renWin.SetSize(300, 300)
    renWin.SetWindowName('CameraBlur')

    # Do the first render and then zoom in a little.
    renWin.Render()
    ren1.GetActiveCamera().SetFocalPoint(0, 0, 0.0)
    ren1.GetActiveCamera().Zoom(1.8)
    ren1.GetActiveCamera().SetFocalDisk(0.05)

    renWin.Render()

    iren.Start()


if __name__ == '__main__':
    main()
