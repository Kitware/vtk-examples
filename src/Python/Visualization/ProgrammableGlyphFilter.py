#!/usr/bin/env python


# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonDataModel import vtkPolyData
from vtkmodules.vtkFiltersProgrammable import vtkProgrammableGlyphFilter
from vtkmodules.vtkFiltersSources import (
    vtkConeSource,
    vtkCubeSource,
    vtkSphereSource
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main(argv):
    colors = vtkNamedColors()

    # Create points.
    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(5, 0, 0)
    points.InsertNextPoint(10, 0, 0)

    # Combine into a polydata.
    polydata = vtkPolyData()
    polydata.SetPoints(points)

    glyph_filter = vtkProgrammableGlyphFilter()
    glyph_filter.SetInputData(polydata)
    # Create the observer.
    observer = CalcGlyph(glyph_filter)
    glyph_filter.SetGlyphMethod(observer)
    # It needs a default glyph, but this should not be used.
    cone_source = vtkConeSource()
    glyph_filter.SetSourceConnection(cone_source.GetOutputPort())

    # Create a mapper and actor.
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(glyph_filter.GetOutputPort())
    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('Gold'))

    # Create a renderer, render window, and interactor.
    renderer = vtkRenderer()
    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(renderer)
    ren_win.SetWindowName('ProgrammableGlyphFilter')

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    # Add the actor to the scene.
    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))

    # Render and interact.
    ren_win.Render()
    renderer.GetActiveCamera().Zoom(0.9)
    iren.Start()


class CalcGlyph(object):
    def __init__(self, glyph_filter):
        self.glyph_filter = glyph_filter

    def __call__(self):
        point_coords = self.glyph_filter.GetPoint()

        print('Calling CalcGlyph for point ', self.glyph_filter.GetPointId())
        print('Point coords are: ', point_coords[0], point_coords[1], point_coords[2])
        if self.glyph_filter.GetPointId() == 0:
            cone_source = vtkConeSource()
            cone_source.SetCenter(point_coords)
            self.glyph_filter.SetSourceConnection(cone_source.GetOutputPort())
        elif self.glyph_filter.GetPointId() == 1:
            cube_source = vtkCubeSource()
            cube_source.SetCenter(point_coords)
            self.glyph_filter.SetSourceConnection(cube_source.GetOutputPort())
        elif self.glyph_filter.GetPointId() == 2:
            sphere_source = vtkSphereSource()
            sphere_source.SetCenter(point_coords)
            self.glyph_filter.SetSourceConnection(sphere_source.GetOutputPort())


if __name__ == '__main__':
    import sys

    main(sys.argv)
