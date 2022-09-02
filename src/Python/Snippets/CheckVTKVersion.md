### Description

This enables us to check the VTK version and provide alternatives for different VTK versions.

`True` is returned if the requested VTK version is >= the current version.

To use the snippet, click the *Copy to clipboard* at the upper right of the code blocks.

### Implementation

``` python

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
    needed_version = 10000000000 * int(major)\
                     + 100000000 * int(minor)\
                     + int(build)
    try:
        vtk_version_number = VTK_VERSION_NUMBER
    except AttributeError:
        # Expand component-wise comparisons for VTK versions < 8.90.
        ver = vtkVersion()
        vtk_version_number = 10000000000 * ver.GetVTKMajorVersion()\
                             + 100000000 * ver.GetVTKMinorVersion()\
                             + ver.GetVTKBuildVersion()
    if vtk_version_number >= needed_version:
        return True
    else:
        return False

```

### Typical usage

``` python

    if vtk_version_ok(*ver):
        try:
            print('Our newest version of the code.')
        except AttributeError:
            pass
    else:
        print('This is code for older versions of VTK.')
    print('Rest of the code.')

```

See:

- [PBR_Skybox](../../Rendering/PBR_Skybox) for a typical usage example. Here we are deciding to use `vtkCameraOrientationWidget` instead of `vtkOrientationMarkerWidget` if the VTK version is >= 9.0.20210718.
- [CheckVTKVersion](../../Utilities/CheckVTKVersion) for a test/example.
