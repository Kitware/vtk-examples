cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(WikiExamples)

# Display type of build
set(_msg "VTKWikiExamples: Building VTKWikiExamples stand-alone")
if(VTK_BINARY_DIR)
  message(STATUS "${_msg} - no")
else()
  message(STATUS "${_msg} - yes")
endif()

# Display build properties
set(_msg "VTKWikiExamples: Setting executables prefix")
message(STATUS "${_msg}")
if(VTK_BINARY_DIR)
  set(WIKI "wiki")
  message(STATUS "${_msg} - yes [prefix: ${WIKI}]")
else()
  set(WIKI "")
  message(STATUS "${_msg} - no")
endif()

set(_vtk_not_found_msg "Unable to find the VTK build folder.")
# Display VTK version
find_package(VTK CONFIG QUIET)
if (NOT VTK_FOUND)
  message(FATAL_ERROR "${_vtk_not_found_msg}")
endif()

if(NOT VTK_BINARY_DIR)
  # if (VTK_VERSION VERSION_LESS "8.90.0")
  #   include(${VTK_USE_FILE})
  # endif()
  if (NOT VTK_FOUND)
    message("Skipping build: ${_vtk_not_found_msg}")
    return ()
  endif()
endif()

if(NOT VTK_RENDERING_BACKEND)
  set(VTK_RENDERING_BACKEND OpenGL2) # Support VTK version prior to introduction of rendering backend
endif()
message(STATUS "VTKWikiExamples: VTK VERSION: ${VTK_VERSION}(${VTK_RENDERING_BACKEND})")

#-----------------------------------------------------------------------------
set(CMAKE_MODULE_PATH
  ${WikiExamples_SOURCE_DIR}/CMake
  ${CMAKE_MODULE_PATH}
  ${VTK_CMAKE_DIR}
  )

#-----------------------------------------------------------------------------
# if (VTK_VERSION VERSION_LESS "8.90.0")
#   include(WikiModuleConfigLegacy)
#   if(WikiModuleConfigLegacy_RETURN)
#     return()
#   endif()
#   include(WikiModuleConfig)
# endif()
include(WikiOutputDirectories)
include(WikiPlatformSpecificChecks)
include(WikiPolicies)
include(WikiTestingConfig)
include(WikiLoadMacros)

#-----------------------------------------------------------------------------
# Mac specific
if(APPLE)
  set(EXECUTABLE_FLAG MACOSX_BUNDLE)
endif()

#-----------------------------------------------------------------------------
add_subdirectory(src/Cxx/Animation)
add_subdirectory(src/Cxx/Annotation)
add_subdirectory(src/Cxx/CompositeData)
if(VTK_USE_MYSQL)
  add_subdirectory(src/Cxx/Databases)
endif()
add_subdirectory(src/Cxx/DataStructures)
add_subdirectory(src/Cxx/Developers)
add_subdirectory(src/Cxx/ExplicitStructuredGrid)
add_subdirectory(src/Cxx/Filtering)
add_subdirectory(src/Cxx/GeometricObjects)
add_subdirectory(src/Cxx/Graphs)
add_subdirectory(src/Cxx/HyperTreeGrid)
add_subdirectory(src/Cxx/ImageData)
add_subdirectory(src/Cxx/ImageProcessing)
add_subdirectory(src/Cxx/Images)
add_subdirectory(src/Cxx/ImplicitFunctions)
add_subdirectory(src/Cxx/InfoVis)
add_subdirectory(src/Cxx/Interaction)
add_subdirectory(src/Cxx/IO)
add_subdirectory(src/Cxx/Lighting)
add_subdirectory(src/Cxx/Math)
add_subdirectory(src/Cxx/Medical)
add_subdirectory(src/Cxx/Meshes)
add_subdirectory(src/Cxx/Modelling)
if(VTK_USE_PARALLEL)
  add_subdirectory(src/Cxx/Parallel)
endif()
add_subdirectory(src/Cxx/Picking)
add_subdirectory(src/Cxx/Plotting)
add_subdirectory(src/Cxx/Points)
add_subdirectory(src/Cxx/PolyData)
add_subdirectory(src/Cxx/RectilinearGrid)
add_subdirectory(src/Cxx/Remote)
add_subdirectory(src/Cxx/Rendering)
add_subdirectory(src/Cxx/RenderMan)
add_subdirectory(src/Cxx/Shaders)
add_subdirectory(src/Cxx/SimpleOperations)
add_subdirectory(src/Cxx/StructuredGrid)
add_subdirectory(src/Cxx/StructuredPoints)
add_subdirectory(src/Cxx/Texture)
add_subdirectory(src/Cxx/Tutorial)
add_subdirectory(src/Cxx/UnstructuredGrid)
add_subdirectory(src/Cxx/Utilities)
add_subdirectory(src/Cxx/Video)
if(VTK_USE_VIEWS OR TARGET VTK::ViewsInfovis )
  add_subdirectory(src/Cxx/Views)
endif()
add_subdirectory(src/Cxx/Visualization)
add_subdirectory(src/Cxx/VisualizationAlgorithms)
add_subdirectory(src/Cxx/VolumeRendering)
add_subdirectory(src/Cxx/Widgets)

if(TARGET VTK::GUISupportQt)
  add_subdirectory(src/Cxx/Qt)
else()
  message(STATUS "VTKWikiExamples: Not building Qt examples")
endif()

# Is git lfs installed?
file(READ "${WikiExamples_SOURCE_DIR}/src/Testing/Data/GitLfsInstalled" data)
if (NOT data STREQUAL "GitLfsInstalled")
     message(STATUS "VTKWikiExamples: git lfs is not installed. Some tests will not be created.")
else ()
     message(STATUS "VTKWikiExamples: git lfs is installed.")
endif()

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")

message(STATUS "")
message(STATUS "Some tests will be excluded.")
message(STATUS "See the CMake files in the folder: ${PROJECT_SOURCE_DIR}/src/ExcludedTests.")
include (${PROJECT_SOURCE_DIR}/src/ExcludedTests/ExcludedTests.cmake)
if(WIN32)
  include (${PROJECT_SOURCE_DIR}/src/ExcludedTests/ExcludedTestsWindows.cmake)
endif()
configure_file(
  "${PROJECT_SOURCE_DIR}/CMake/CTestCustom.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake"
  @ONLY)
message(STATUS "")
