#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkIOImage import vtkPNGWriter
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkWindowToImageFilter
)


def main():
    colors = vtkNamedColors()

    # create a rendering window and renderer
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)
    renWin.SetWindowName('Screenshot')

    # create a renderwindowinteractor
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # create source
    source = vtkSphereSource()
    source.SetCenter(0, 0, 0)
    source.SetRadius(5.0)
    source.SetPhiResolution(30)
    source.SetThetaResolution(30)

    # mapper
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(source.GetOutputPort())

    # actor
    actor = vtkActor()
    actor.GetProperty().SetColor(colors.GetColor3d('IndianRed'))
    actor.GetProperty().SetSpecular(0.6)
    actor.GetProperty().SetSpecularPower(30)
    actor.SetMapper(mapper)

    # assign actor to the renderer
    ren.AddActor(actor)
    ren.SetBackground(colors.GetColor3d('MistyRose'))

    renWin.Render()

    # screenshot code:
    w2if = vtkWindowToImageFilter()
    w2if.SetInput(renWin)
    w2if.SetInputBufferTypeToRGB()
    w2if.ReadFrontBufferOff()
    w2if.Update()

    writer = vtkPNGWriter()
    writer.SetFileName('TestScreenshot.png')
    writer.SetInputConnection(w2if.GetOutputPort())
    writer.Write()

    # enable user interface interactor
    iren.Initialize()
    iren.Start()


if __name__ == '__main__':
    main()
