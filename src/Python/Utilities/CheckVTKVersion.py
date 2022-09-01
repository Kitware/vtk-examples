#!/usr/bin/env python3

from vtkmodules.vtkCommonCore import (
    VTK_VERSION_NUMBER,
    vtkVersion
)


def vtk_version_ok(major, minor, build):
    """
    Check the VTK version.

    :param major: Major version.
    :param minor: Minor version.
    :param build: Build version.
    :return: True if the requested VTK version is greater or equal to the actual VTK version.
    """
    needed_version = 10000000000 * int(major) \
                     + 100000000 * int(minor) \
                     + int(build)
    try:
        vtk_version_number = VTK_VERSION_NUMBER
    except AttributeError:
        # Expand component-wise comparisons for VTK versions < 8.90.
        ver = vtkVersion()
        vtk_version_number = 10000000000 * ver.GetVTKMajorVersion() \
                             + 100000000 * ver.GetVTKMinorVersion() \
                             + ver.GetVTKBuildVersion()
    if vtk_version_number >= needed_version:
        return True
    else:
        return False


def main():
    test_versions = ((9, 2, 20220831), (10, 99, 0))
    for ver in test_versions:
        print('Checking VTK version >=', '.'.join(map(str, ver)))
        if vtk_version_ok(*ver):
            try:
                print('This code only works for VTK Versions >=', '.'.join(map(str, ver)))
            except AttributeError:
                print('Unable to execute the new code.')
                pass
        else:
            print('This is alternative code for older versions of VTK.')
            print('It may not work for VTK Versions >=', '.'.join(map(str, ver)))
        print('Rest of the code in this section.')
        print(vtkVersion.GetVTKSourceVersion())
        print()


if __name__ == '__main__':
    main()
