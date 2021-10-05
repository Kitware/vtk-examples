#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkStructuredGridOutlineFilter
from vtkmodules.vtkFiltersFlowPaths import vtkStreamTracer
from vtkmodules.vtkFiltersSources import vtkPlaneSource
from vtkmodules.vtkIOParallel import vtkMultiBlockPLOT3DReader
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def get_program_parameters():
    import argparse
    description = 'Streamlines.'
    epilogue = '''
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('xyz_file', help='combxyz.bin.')
    parser.add_argument('q_file', help='combq.bin.')
    args = parser.parse_args()
    return args.xyz_file, args.q_file


def main():
    colors = vtkNamedColors()

    # colors.SetColor('bkg', [0.1, 0.2, 0.4, 1.0])

    xyz_file, q_file = get_program_parameters()

    # Read the data.
    #
    pl3d = vtkMultiBlockPLOT3DReader()
    pl3d.SetXYZFileName(xyz_file)
    pl3d.SetQFileName(q_file)
    pl3d.SetScalarFunctionNumber(100)
    pl3d.SetVectorFunctionNumber(202)
    pl3d.Update()

    seeds = vtkPlaneSource()
    seeds.SetXResolution(4)
    seeds.SetYResolution(4)
    seeds.SetOrigin(2, -2, 26)
    seeds.SetPoint1(2, 2, 26)
    seeds.SetPoint2(2, -2, 32)

    streamline = vtkStreamTracer()
    streamline.SetInputData(pl3d.GetOutput().GetBlock(0))
    streamline.SetSourceConnection(seeds.GetOutputPort())
    streamline.SetMaximumPropagation(200)
    streamline.SetInitialIntegrationStep(.2)
    streamline.SetIntegrationDirectionToForward()

    streamline_mapper = vtkPolyDataMapper()
    streamline_mapper.SetInputConnection(streamline.GetOutputPort())
    streamline_actor = vtkActor()
    streamline_actor.SetMapper(streamline_mapper)
    streamline_actor.VisibilityOn()

    outline = vtkStructuredGridOutlineFilter()
    outline.SetInputData(pl3d.GetOutput().GetBlock(0))
    outline_mapper = vtkPolyDataMapper()
    outline_mapper.SetInputConnection(outline.GetOutputPort())
    outline_actor = vtkActor()
    outline_actor.SetMapper(outline_mapper)
    outline_actor.GetProperty().SetColor(colors.GetColor3d('White'))

    renderer = vtkRenderer()
    render_window = vtkRenderWindow()
    render_window.AddRenderer(renderer)
    render_window.SetWindowName('StreamLines')

    interactor = vtkRenderWindowInteractor()
    interactor.SetInteractorStyle(vtkInteractorStyleTrackballCamera())
    render_window.SetInteractor(interactor)

    renderer.AddActor(streamline_actor)
    renderer.AddActor(outline_actor)

    renderer.SetBackground(colors.GetColor3d('MidnightBlue'))
    interactor.Initialize()
    render_window.Render()
    renderer.GetActiveCamera().SetPosition(-32.8, -12.3, 46.3)
    renderer.GetActiveCamera().SetFocalPoint(8.3, 0.03, 29.8)
    renderer.GetActiveCamera().SetViewUp(0.2, 0.5, 0.9)
    render_window.Render()
    interactor.Start()


if __name__ == '__main__':
    main()
