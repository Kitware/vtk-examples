#include <vtkNew.h>
#include <vtkVersion.h>

#include <array>
#include <vector>

#ifdef VTK_VERSION_NUMBER
#if VTK_VERSION_NUMBER >= 90020210809ULL
#define VTK_HAS_COW 1
#endif
#endif

#if VTK_HAS_COW
// Add any specific includes here.
// #include <vtkCameraOrientationWidget.h>
#endif

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

int main(int, char*[])
{
  std::cout << "VTK Version: " << vtkVersion::GetVTKVersion() << std::endl;

  // We can check and exit if the VTK version is too old.
  if (!VTKVersionOk(9, 0, 0))
  {
    std::cerr << "You need VTK version 9.0.0 or greater to run this program."
              << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<std::array<int, 3>> testVersions = {{9, 2, 20220831}, {9, 19, 0}};
  for (auto const& ver : testVersions)
  {
    if (VTKVersionOk(ver[0], ver[1], ver[2]))
    {
      std::cout << "This code works for VTK Versions >= " << ver[0] << "."
                << ver[1] << "." << ver[2] << std::endl;
    }
    else
    {
      std::cout << "You need VTK Version " << ver[0] << "." << ver[1] << "."
                << ver[2] << " or greater." << std::endl;
    }
  }

  // If we want to use alternative code do it this way.
#ifdef VTK_HAS_COW
  std::cout << "Using vtkCameraOrientationWidget." << std::endl;
#else
  std::cout << "Fallback to using vtkAxesActor" << std::endl;
#endif

  return EXIT_SUCCESS;
}

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
