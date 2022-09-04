### Description

This enables us to check the VTK version and provide alternatives for different VTK versions.

`true` is returned if the requested VTK version is >= the current version.

To use the snippet, click the *Copy to clipboard* at the upper right of the code blocks.

### Declaration Section

``` c++
#include <vtkNew.h>
#include <vtkVersion.h>

namespace {
/**
 * Check the VTK version.
 *
 * @param major: Major version.
 * @param major: Minor version.
 * @param major: Build version.
 *
 * @return true if the requested VTK version is greater or equal to the actual
 * VTK version.
 */
bool VTKVersionOk(unsigned long long const& major,
                  unsigned long long const& minor,
                  unsigned long long const& build);

} // namespace
```

### Implementation Section

``` c++

namespace {
bool VTKVersionOk(unsigned long long const& major,
                  unsigned long long const& minor,
                  unsigned long long const& build)
{
  unsigned long long neededVersion =
      10000000000ULL * major + 100000000ULL * minor + build;
  auto res = false;
#ifndef VTK_VERSION_NUMBER
  vtkNew<vtkVersion> ver;
  unsigned long long vtk_version_number =
      10000000000ULL * ver->GetVTKMajorVersion() +
      100000000ULL * ver->GetVTKMinorVersion() + ver->GetVTKBuildVersion();
  res = vtk_version_number >= neededVersion;
#else
  res = VTK_VERSION_NUMBER >= neededVersion;
#endif
  return res;
}

} // namespace
```

### Typical usage

``` c++

  // We can check and exit if the VTK version is too old.
  if (!VTKVersionOk(9, 0, 0))
  {
    std::cerr << "You need VTK version 9.0.0 or greater to run this program."
              << std::endl;
    return EXIT_FAILURE;
  }


```

See:

- [PBR_Skybox](../../Rendering/PBR_Skybox) for a typical usage example. Here we are deciding to use `vtkCameraOrientationWidget` instead of `vtkOrientationMarkerWidget` if the VTK version is >= 9.0.20210718.
- [CheckVTKVersion](../../Utilities/CheckVTKVersion) for a test/example.
