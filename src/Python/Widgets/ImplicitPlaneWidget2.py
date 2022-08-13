#!/usr/bin/env python3

from pathlib import Path

# You may need to uncomment one or more of the following imports.
# If vtkRenderWindow is used and you want to use OpenGL,
#   you also need the vtkRenderingOpenGL2 module.
# If vtkRenderWindowInteractor is used, uncomment vtkInteractionStyle
# If text rendering is used, uncomment vtkRenderingFreeType.
#
# If using PyCharm, preface each one you select with this line:
# noinspection PyUnresolvedReferences
#
# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
# import vtkmodules.vtkRenderingContextOpenGL2
# import vtkmodules.vtkRenderingFreeType
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingUI
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkCommand
from vtkmodules.vtkCommonDataModel import vtkPlane
from vtkmodules.vtkFiltersCore import (
    vtkClipPolyData
)
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkIOXML import vtkXMLPolyDataReader
from vtkmodules.vtkInteractionWidgets import (
    vtkImplicitPlaneRepresentation,
    vtkImplicitPlaneWidget2
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkProperty,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main(fn):
    colors = vtkNamedColors()
    sphere_source = vtkSphereSource()
    sphere_source.SetRadius(10.0)

    fp = None
    if fn:
        fp = Path(fn)
        if not (fp.is_file() and fp.suffix == '.vtp'):
            print('Expected an existing file name with extension .vtp:')
            print('Got', fp)
            return

    # Setup a visualization pipeline.
    plane = vtkPlane()
    clipper = vtkClipPolyData()
    clipper.SetClipFunction(plane)
    clipper.InsideOutOn()
    if fn:
        reader = vtkXMLPolyDataReader()
        reader.SetFileName(fp)
        clipper.SetInputConnection(reader.GetOutputPort())
    else:
        clipper.SetInputConnection(sphere_source.GetOutputPort())

    # Create a mapper and actor.
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(clipper.GetOutputPort())
    actor = vtkActor()
    actor.SetMapper(mapper)

    back_faces = vtkProperty()
    back_faces.SetDiffuseColor(colors.GetColor3d('Gold'))

    actor.SetBackfaceProperty(back_faces)

    # A renderer and render window
    renderer = vtkRenderer()
    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(renderer)
    ren_win.SetWindowName('ImplicitPlaneWidget2')

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))

    # An interactor
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    # The callback will do the work.
    my_callback = IPWCallback(plane)

    rep = vtkImplicitPlaneRepresentation()
    rep.SetPlaceFactor(1.25)  # This must be set prior to placing the widget
    rep.PlaceWidget(actor.GetBounds())
    rep.SetNormal(plane.GetNormal())

    plane_widget = vtkImplicitPlaneWidget2()
    plane_widget.SetInteractor(iren)
    plane_widget.SetRepresentation(rep)
    plane_widget.AddObserver(vtkCommand.InteractionEvent, my_callback)

    renderer.GetActiveCamera().Azimuth(-60)
    renderer.GetActiveCamera().Elevation(30)
    renderer.ResetCamera()
    renderer.GetActiveCamera().Zoom(0.75)

    # Render and interact.
    iren.Initialize()
    ren_win.Render()
    plane_widget.On()

    # Begin mouse interaction.
    iren.Start()


class IPWCallback:
    def __init__(self, plane):
        self.plane = plane

    def __call__(self, caller, ev):
        rep = caller.GetRepresentation()
        rep.GetPlane(self.plane)


def get_program_parameters():
    import argparse
    description = 'How to use the second generation ImplicitPlaneWidget2 to interactively' \
                  ' define the clipping plane for a polydata.'
    epilogue = '''
    If no arguments are specified, a vtkSphereSource generates the polydata.
    By specifying a .vtp file, the example can operate on arbitrary polydata.
'''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('file_name', nargs='?', default=None, help='A VTK Poly Data file e.g. cow.vtp')

    args = parser.parse_args()
    return args.file_name


if __name__ == '__main__':
    file_name = get_program_parameters()
    main(file_name)
