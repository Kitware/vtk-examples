#!/usr/bin/env python

from pathlib import Path

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    VTK_VERSION_NUMBER,
    vtkLookupTable,
    vtkVersion
)
from vtkmodules.vtkFiltersCore import (
    vtkFlyingEdges3D,
    vtkMarchingCubes,
    vtkPolyDataNormals,
    vtkStripper,
    vtkWindowedSincPolyDataFilter
)
from vtkmodules.vtkIOImage import vtkMetaImageReader
from vtkmodules.vtkImagingCore import vtkImageThreshold
from vtkmodules.vtkImagingGeneral import vtkImageGaussianSmooth
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def get_program_parameters(argv):
    import argparse
    description = 'The frog\'s brain.'
    epilogue = '''
    Model extracted without smoothing (left) and with smoothing (right).
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('data_folder', help='The path to the file: frogtissue.mhd.')
    parser.add_argument('tissue', default='brain', nargs='?', help='The tissue to use.')
    args = parser.parse_args()
    return args.data_folder, args.tissue


def main(data_folder, tissue):
    # vtkFlyingEdges3D was introduced in VTK >= 8.2
    use_flying_edges = vtk_version_ok(8, 2, 0)

    colors = vtkNamedColors()

    path = Path(data_folder)
    if path.is_dir():
        s = ''
        file_name = path.joinpath('frogtissue').with_suffix('.mhd')
        if not file_name.is_file():
            s += 'The file: {:s} does not exist.'.format(str(file_name))
        if s:
            print(s)
            return
    else:
        print('Expected a path to frogtissue.mhd')
        return

    tissue_map = create_tissue_map()
    lut = create_frog_lut(colors)

    # Setup render window, renderer, and interactor.
    renderer_left = vtkRenderer()
    renderer_left.SetViewport(0, 0, 0.5, 1)
    renderer_right = vtkRenderer()
    renderer_right.SetViewport(0.5, 0, 1, 1)
    render_window = vtkRenderWindow()
    render_window.AddRenderer(renderer_left)
    render_window.AddRenderer(renderer_right)
    render_window_interactor = vtkRenderWindowInteractor()
    render_window_interactor.SetRenderWindow(render_window)

    actor = create_frog_actor(file_name, tissue_map[tissue], use_flying_edges)
    actor.GetProperty().SetDiffuseColor(lut.GetTableValue(tissue_map[tissue])[:3])
    renderer_left.AddActor(actor)

    actor_smooth = create_smooth_frog_actor(file_name, tissue_map[tissue], use_flying_edges)
    actor_smooth.GetProperty().SetDiffuseColor(lut.GetTableValue(tissue_map[tissue])[:3])
    actor_smooth.GetProperty().SetDiffuse(1.0)
    actor_smooth.GetProperty().SetSpecular(.5)
    actor_smooth.GetProperty().SetSpecularPower(100)

    renderer_right.AddActor(actor_smooth)

    renderer_left.ResetCamera()
    renderer_left.GetActiveCamera().SetViewUp(-1, 0, 0)
    renderer_left.GetActiveCamera().Azimuth(180)
    renderer_left.ResetCameraClippingRange()

    renderer_left.SetBackground(colors.GetColor3d('LightSteelBlue'))
    renderer_right.SetBackground(colors.GetColor3d('LightSteelBlue'))
    renderer_right.SetActiveCamera(renderer_left.GetActiveCamera())

    render_window.SetSize(640, 480)
    render_window.SetWindowName('FrogBrain')
    render_window.Render()

    render_window_interactor.Start()


def create_smooth_frog_actor(file_name, tissue, use_flying_edges):
    reader = vtkMetaImageReader()
    reader.SetFileName(str(file_name))
    reader.Update()

    select_tissue = vtkImageThreshold()
    select_tissue.ThresholdBetween(tissue, tissue)
    select_tissue.SetInValue(255)
    select_tissue.SetOutValue(0)
    select_tissue.SetInputConnection(reader.GetOutputPort())

    gaussian_radius = 1
    gaussian_standard_deviation = 2.0
    gaussian = vtkImageGaussianSmooth()
    gaussian.SetStandardDeviations(gaussian_standard_deviation, gaussian_standard_deviation,
                                   gaussian_standard_deviation)
    gaussian.SetRadiusFactors(gaussian_radius, gaussian_radius, gaussian_radius)
    gaussian.SetInputConnection(select_tissue.GetOutputPort())

    # iso_value = 63.5
    iso_value = 127.5
    if use_flying_edges:
        try:
            iso_surface = vtkFlyingEdges3D()
        except AttributeError:
            iso_surface = vtkMarchingCubes()
    else:
        iso_surface = vtkMarchingCubes()
    iso_surface.SetInputConnection(gaussian.GetOutputPort())
    iso_surface.ComputeScalarsOff()
    iso_surface.ComputeGradientsOff()
    iso_surface.ComputeNormalsOff()
    iso_surface.SetValue(0, iso_value)

    smoothing_iterations = 20
    pass_band = 0.001
    feature_angle = 60.0
    smoother = vtkWindowedSincPolyDataFilter()
    smoother.SetInputConnection(iso_surface.GetOutputPort())
    smoother.SetNumberOfIterations(smoothing_iterations)
    smoother.BoundarySmoothingOff()
    smoother.FeatureEdgeSmoothingOff()
    smoother.SetFeatureAngle(feature_angle)
    smoother.SetPassBand(pass_band)
    smoother.NonManifoldSmoothingOn()
    smoother.NormalizeCoordinatesOff()
    smoother.Update()

    normals = vtkPolyDataNormals()
    normals.SetInputConnection(smoother.GetOutputPort())
    normals.SetFeatureAngle(feature_angle)

    stripper = vtkStripper()
    stripper.SetInputConnection(normals.GetOutputPort())

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(stripper.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)

    return actor


def create_frog_actor(file_name, tissue, use_flying_edges):
    reader = vtkMetaImageReader()
    reader.SetFileName(str(file_name))
    reader.Update()

    select_tissue = vtkImageThreshold()
    select_tissue.ThresholdBetween(tissue, tissue)
    select_tissue.SetInValue(255)
    select_tissue.SetOutValue(0)
    select_tissue.SetInputConnection(reader.GetOutputPort())

    iso_value = 63.5
    if use_flying_edges:
        try:
            iso_surface = vtkFlyingEdges3D()
        except AttributeError:
            iso_surface = vtkMarchingCubes()
    else:
        iso_surface = vtkMarchingCubes()
    iso_surface.SetInputConnection(select_tissue.GetOutputPort())
    iso_surface.ComputeScalarsOff()
    iso_surface.ComputeGradientsOff()
    iso_surface.ComputeNormalsOn()
    iso_surface.SetValue(0, iso_value)

    stripper = vtkStripper()
    stripper.SetInputConnection(iso_surface.GetOutputPort())

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(stripper.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)

    return actor


def create_frog_lut(colors):
    lut = vtkLookupTable()
    lut.SetNumberOfColors(16)
    lut.SetTableRange(0, 15)
    lut.Build()

    lut.SetTableValue(0, colors.GetColor4d('Black'))
    lut.SetTableValue(1, colors.GetColor4d('salmon'))  # blood
    lut.SetTableValue(2, colors.GetColor4d('beige'))  # brain
    lut.SetTableValue(3, colors.GetColor4d('orange'))  # duodenum
    lut.SetTableValue(4, colors.GetColor4d('misty_rose'))  # eye_retina
    lut.SetTableValue(5, colors.GetColor4d('white'))  # eye_white
    lut.SetTableValue(6, colors.GetColor4d('tomato'))  # heart
    lut.SetTableValue(7, colors.GetColor4d('raspberry'))  # ileum
    lut.SetTableValue(8, colors.GetColor4d('banana'))  # kidney
    lut.SetTableValue(9, colors.GetColor4d('peru'))  # l_intestine
    lut.SetTableValue(10, colors.GetColor4d('pink'))  # liver
    lut.SetTableValue(11, colors.GetColor4d('powder_blue'))  # lung
    lut.SetTableValue(12, colors.GetColor4d('carrot'))  # nerve
    lut.SetTableValue(13, colors.GetColor4d('wheat'))  # skeleton
    lut.SetTableValue(14, colors.GetColor4d('violet'))  # spleen
    lut.SetTableValue(15, colors.GetColor4d('plum'))  # stomach

    return lut


def create_tissue_map():
    tissue_map = dict()
    tissue_map['blood'] = 1
    tissue_map['brain'] = 2
    tissue_map['duodenum'] = 3
    tissue_map['eyeRetina'] = 4
    tissue_map['eyeWhite'] = 5
    tissue_map['heart'] = 6
    tissue_map['ileum'] = 7
    tissue_map['kidney'] = 8
    tissue_map['intestine'] = 9
    tissue_map['liver'] = 10
    tissue_map['lung'] = 11
    tissue_map['nerve'] = 12
    tissue_map['skeleton'] = 13
    tissue_map['spleen'] = 14
    tissue_map['stomach'] = 15

    return tissue_map


def vtk_version_ok(major, minor, build):
    """
    Check the VTK version.

    :param major: Major version.
    :param minor: Minor version.
    :param build: Build version.
    :return: True if the requested VTK version is greater or equal to the actual VTK version.
    """
    needed_version = 10000000000 * int(major) + 100000000 * int(minor) + int(build)
    try:
        vtk_version_number = VTK_VERSION_NUMBER
    except AttributeError:  # as error:
        ver = vtkVersion()
        vtk_version_number = 10000000000 * ver.GetVTKMajorVersion() + 100000000 * ver.GetVTKMinorVersion() \
                             + ver.GetVTKBuildVersion()
    if vtk_version_number >= needed_version:
        return True
    else:
        return False


if __name__ == '__main__':
    import sys

    data_folder, tissue = get_program_parameters(sys.argv)

    main(data_folder, tissue)
