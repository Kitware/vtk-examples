#!/usr/bin/env python

# Python example translated directly from Tcl test
# [vtk_source]/Graphics/Testing/Tcl/progGlyphs.tcl

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkElevationFilter
from vtkmodules.vtkFiltersProgrammable import vtkProgrammableGlyphFilter
from vtkmodules.vtkFiltersSources import (
    vtkPlaneSource,
    vtkSuperquadricSource
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    res = 6
    plane = vtkPlaneSource()
    plane.SetResolution(res, res)
    colors = vtkElevationFilter()
    colors.SetInputConnection(plane.GetOutputPort())
    colors.SetLowPoint(-0.25, -0.25, -0.25)
    colors.SetHighPoint(0.25, 0.25, 0.25)
    planeMapper = vtkPolyDataMapper()
    planeMapper.SetInputData(colors.GetPolyDataOutput())
    planeActor = vtkActor()
    planeActor.SetMapper(planeMapper)
    planeActor.GetProperty().SetRepresentationToWireframe()

    # create simple poly data so we can apply glyph
    squad = vtkSuperquadricSource()

    def Glyph():
        """
        # procedure for generating glyphs
        :return:
        """
        xyz = glypher.GetPoint()
        x = xyz[0]
        y = xyz[1]
        length = glypher.GetInput(0).GetLength()
        scale = length / (2.0 * res)

        squad.SetScale(scale, scale, scale)
        squad.SetCenter(xyz)
        squad.SetPhiRoundness(abs(x) * 5.0)
        squad.SetThetaRoundness(abs(y) * 5.0)

    glypher = vtkProgrammableGlyphFilter()
    glypher.SetInputConnection(colors.GetOutputPort())
    glypher.SetSourceConnection(squad.GetOutputPort())
    glypher.SetGlyphMethod(Glyph)
    glyphMapper = vtkPolyDataMapper()
    glyphMapper.SetInputConnection(glypher.GetOutputPort())
    glyphActor = vtkActor()
    glyphActor.SetMapper(glyphMapper)

    colors = vtkNamedColors()

    # Create the rendering stuff
    ren1 = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.SetMultiSamples(0)
    renWin.AddRenderer(ren1)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)
    ren1.AddActor(planeActor)
    ren1.AddActor(glyphActor)
    ren1.SetBackground(colors.GetColor3d('Silver'))

    renWin.SetSize(450, 450)
    renWin.SetWindowName('ProgrammableGlyphs')
    renWin.Render()

    ren1.GetActiveCamera().Zoom(1.3)

    iren.Start()


if __name__ == '__main__':
    main()
