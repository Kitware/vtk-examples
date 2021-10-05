#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    VTK_VERSION_NUMBER,
    vtkVersion
)
from vtkmodules.vtkCommonDataModel import (
    vtkCylinder,
    vtkSphere
)
from vtkmodules.vtkFiltersCore import (
    vtkFlyingEdges3D,
    vtkMarchingCubes,
    vtkProbeFilter
)
from vtkmodules.vtkImagingHybrid import vtkSampleFunction
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    # vtkFlyingEdges3D was introduced in VTK >= 8.2
    use_flying_edges = vtk_version_ok(8, 2, 0)

    colors = vtkNamedColors()

    sample_resolution = get_program_parameters()

    # Create a sampled sphere
    implicit_sphere = vtkSphere()
    radius = 1.0
    implicit_sphere.SetRadius(radius)

    sampled_sphere = vtkSampleFunction()
    sampled_sphere.SetSampleDimensions(sample_resolution, sample_resolution, sample_resolution)
    x_min = -radius * 2.0
    x_max = radius * 2.0
    sampled_sphere.SetModelBounds(x_min, x_max, x_min, x_max, x_min, x_max)
    sampled_sphere.SetImplicitFunction(implicit_sphere)

    if use_flying_edges:
        try:
            iso_sphere = vtkFlyingEdges3D()
        except AttributeError:
            iso_sphere = vtkMarchingCubes()
    else:
        iso_sphere = vtkMarchingCubes()
    iso_sphere.SetValue(0, 1.0)
    iso_sphere.SetInputConnection(sampled_sphere.GetOutputPort())

    # Create a sampled cylinder
    implicit_cylinder = vtkCylinder()
    implicit_cylinder.SetRadius(radius / 2.0)
    sampled_cylinder = vtkSampleFunction()
    sampled_cylinder.SetSampleDimensions(sample_resolution, sample_resolution, sample_resolution)
    sampled_cylinder.SetModelBounds(x_min, x_max, x_min, x_max, x_min, x_max)
    sampled_cylinder.SetImplicitFunction(implicit_cylinder)

    # Probe cylinder with the sphere isosurface
    probe_cylinder = vtkProbeFilter()
    probe_cylinder.SetInputConnection(0, iso_sphere.GetOutputPort())
    probe_cylinder.SetInputConnection(1, sampled_cylinder.GetOutputPort())
    probe_cylinder.Update()

    # Restore the original normals
    probe_cylinder.GetOutput().GetPointData().SetNormals(
        iso_sphere.GetOutput().GetPointData().GetNormals())

    print('Scalar range: {:6.3f}, {:6.3f}'.format(probe_cylinder.GetOutput().GetScalarRange()[0],
                                                  probe_cylinder.GetOutput().GetScalarRange()[1]))

    # Create a mapper and actor
    map_sphere = vtkPolyDataMapper()
    map_sphere.SetInputConnection(probe_cylinder.GetOutputPort())
    map_sphere.SetScalarRange(probe_cylinder.GetOutput().GetScalarRange())

    sphere = vtkActor()
    sphere.SetMapper(map_sphere)

    # Visualize
    renderer = vtkRenderer()
    render_window = vtkRenderWindow()
    render_window.AddRenderer(renderer)
    render_window.SetWindowName('IsosurfaceSampling')

    render_window_interactor = vtkRenderWindowInteractor()
    render_window_interactor.SetRenderWindow(render_window)

    renderer.AddActor(sphere)
    renderer.SetBackground(colors.GetColor3d('AliceBlue'))

    render_window.Render()
    render_window_interactor.Start()


def get_program_parameters():
    import argparse
    description = 'Create an isosurface and create point data on that isosurface that is sampled from another dataset.'
    epilogue = '''
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-r', '--resolution', type=int, default=50,
                        help='The sample resolution of the sphere and cylinder')
    args = parser.parse_args()
    return args.resolution


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
    main()
