#!/usr/bin/env python

import vtkmodules.all as vtk


def main():
    # vtkFlyingEdges3D was introduced in VTK >= 8.2
    use_flying_edges = vtk_version_ok(8, 2, 0)

    colors = vtk.vtkNamedColors()

    sample_resolution = get_program_parameters()

    # Create a sampled sphere
    implicit_sphere = vtk.vtkSphere()
    radius = 1.0
    implicit_sphere.SetRadius(radius)

    sampled_sphere = vtk.vtkSampleFunction()
    sampled_sphere.SetSampleDimensions(sample_resolution, sample_resolution, sample_resolution)
    x_min = -radius * 2.0
    x_max = radius * 2.0
    sampled_sphere.SetModelBounds(x_min, x_max, x_min, x_max, x_min, x_max)
    sampled_sphere.SetImplicitFunction(implicit_sphere)

    if use_flying_edges:
        try:
            iso_sphere = vtk.vtkFlyingEdges3D()
        except AttributeError:
            iso_sphere = vtk.vtkMarchingCubes()
    else:
        iso_sphere = vtk.vtkMarchingCubes()
    iso_sphere.SetValue(0, 1.0)
    iso_sphere.SetInputConnection(sampled_sphere.GetOutputPort())

    # Create a sampled cylinder
    implicit_cylinder = vtk.vtkCylinder()
    implicit_cylinder.SetRadius(radius / 2.0)
    sampled_cylinder = vtk.vtkSampleFunction()
    sampled_cylinder.SetSampleDimensions(sample_resolution, sample_resolution, sample_resolution)
    sampled_cylinder.SetModelBounds(x_min, x_max, x_min, x_max, x_min, x_max)
    sampled_cylinder.SetImplicitFunction(implicit_cylinder)

    # Probe cylinder with the sphere isosurface
    probe_cylinder = vtk.vtkProbeFilter()
    probe_cylinder.SetInputConnection(0, iso_sphere.GetOutputPort())
    probe_cylinder.SetInputConnection(1, sampled_cylinder.GetOutputPort())
    probe_cylinder.Update()

    # Restore the original normals
    probe_cylinder.GetOutput().GetPointData().SetNormals(
        iso_sphere.GetOutput().GetPointData().GetNormals())

    print('Scalar range: {:6.3f}, {:6.3f}'.format(probe_cylinder.GetOutput().GetScalarRange()[0],
                                                  probe_cylinder.GetOutput().GetScalarRange()[1]))

    # Create a mapper and actor
    map_sphere = vtk.vtkPolyDataMapper()
    map_sphere.SetInputConnection(probe_cylinder.GetOutputPort())
    map_sphere.SetScalarRange(probe_cylinder.GetOutput().GetScalarRange())

    sphere = vtk.vtkActor()
    sphere.SetMapper(map_sphere)

    # Visualize
    renderer = vtk.vtkRenderer()
    render_window = vtk.vtkRenderWindow()
    render_window.AddRenderer(renderer)
    render_window.SetWindowName('IsosurfaceSampling')

    render_window_interactor = vtk.vtkRenderWindowInteractor()
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
        vtk_version_number = vtk.VTK_VERSION_NUMBER
    except AttributeError:  # as error:
        ver = vtk.vtkVersion()
        vtk_version_number = 10000000000 * ver.GetVTKMajorVersion() + 100000000 * ver.GetVTKMinorVersion() \
                             + ver.GetVTKBuildVersion()
    if vtk_version_number >= needed_version:
        return True
    else:
        return False


if __name__ == '__main__':
    main()
