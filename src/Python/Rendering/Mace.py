#!/usr/bin/env python

import vtk


def main():
    colors = vtk.vtkNamedColors()

    renderer = vtk.vtkRenderer()

    renWin = vtk.vtkRenderWindow()
    renWin.AddRenderer(renderer)

    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    sphere = vtk.vtkSphereSource()
    sphere.SetThetaResolution(8)
    sphere.SetPhiResolution(8)

    sphereMapper = vtk.vtkPolyDataMapper()
    sphereMapper.SetInputConnection(sphere.GetOutputPort())

    sphereActor = vtk.vtkActor()
    sphereActor.SetMapper(sphereMapper)
    sphereActor.GetProperty().SetColor(colors.GetColor3d('Silver'))

    cone = vtk.vtkConeSource()
    cone.SetResolution(6)

    glyph = vtk.vtkGlyph3D()
    glyph.SetInputConnection(sphere.GetOutputPort())
    glyph.SetSourceConnection(cone.GetOutputPort())
    glyph.SetVectorModeToUseNormal()
    glyph.SetScaleModeToScaleByVector()
    glyph.SetScaleFactor(0.25)

    spikeMapper = vtk.vtkPolyDataMapper()
    spikeMapper.SetInputConnection(glyph.GetOutputPort())

    spikeActor = vtk.vtkActor()
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
