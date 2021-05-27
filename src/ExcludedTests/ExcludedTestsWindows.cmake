# These are tests to be excluded from testing that are specific to Windows
# Meshes-InterpolateFieldDataDemo: Test file has UTF-8 characters,
#  fails in Windows. See: https://gitlab.kitware.com/vtk/vtk/-/issues/18026
#  and https://discourse.vtk.org/t/vtkexodusiireader-possible-bug/3346/2
# Rendering-StringToImageDemo: Bar on right side in Windows
list(APPEND TESTS_TO_IGNORE
  "Meshes-InterpolateFieldDataDemo"
  "Rendering-StringToImageDemo"
  )
