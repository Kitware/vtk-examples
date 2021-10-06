#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkSuperquadricSource
from vtkmodules.vtkIOXML import vtkXMLPolyDataReader
from vtkmodules.vtkInteractionWidgets import vtkOrientationMarkerWidget
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    file_name = get_program_parameters()

    # Read the polydata for the icon
    reader = vtkXMLPolyDataReader()
    reader.SetFileName(file_name)

    icon_mapper = vtkDataSetMapper()
    icon_mapper.SetInputConnection(reader.GetOutputPort())

    icon_actor = vtkActor()
    icon_actor.SetMapper(icon_mapper)
    icon_actor.GetProperty().SetColor(colors.GetColor3d('Silver'))

    # Set up the renderer, window, and interactor
    renderer = vtkRenderer()
    renderer.SetBackground(colors.GetColor3d('SlateGray'))

    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(renderer)
    ren_win.SetSize(400, 400)
    ren_win.SetWindowName('OrientationMarkerWidget1')

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    rgb = [0.0, 0.0, 0.0]
    colors.GetColorRGB('Wheat', rgb)
    # Set up the widget
    widget = vtkOrientationMarkerWidget()
    widget.SetOrientationMarker(icon_actor)
    widget.SetInteractor(iren)
    widget.SetViewport(0.0, 0.0, 0.2, 0.2)
    widget.SetOutlineColor(*rgb)
    widget.SetEnabled(1)
    widget.InteractiveOn()

    # Create a superquadric
    superquadric_source = vtkSuperquadricSource()
    superquadric_source.SetPhiRoundness(.2)
    superquadric_source.SetThetaRoundness(.8)

    # Create a mapper and actor
    superquadric_mapper = vtkPolyDataMapper()
    superquadric_mapper.SetInputConnection(superquadric_source.GetOutputPort())

    superquadric_actor = vtkActor()
    superquadric_actor.SetMapper(superquadric_mapper)
    superquadric_actor.GetProperty().SetInterpolationToFlat()
    superquadric_actor.GetProperty().SetDiffuseColor(colors.GetColor3d('Carrot'))
    superquadric_actor.GetProperty().SetSpecularColor(colors.GetColor3d('White'))
    superquadric_actor.GetProperty().SetDiffuse(0.6)
    superquadric_actor.GetProperty().SetSpecular(0.5)
    superquadric_actor.GetProperty().SetSpecularPower(5.0)

    renderer.AddActor(superquadric_actor)
    renderer.ResetCamera()

    ren_win.Render()

    iren.Initialize()

    iren.Start()


def get_program_parameters():
    import argparse
    description = 'OrientationMarkerWidget1'
    epilogue = """
   """
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('file_name', help='Bunny.vtp')
    args = parser.parse_args()
    return args.file_name


if __name__ == '__main__':
    main()
