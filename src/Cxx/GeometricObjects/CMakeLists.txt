project (${WIKI}GeometricObjects)

if(NOT VTK_BINARY_DIR)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    CommonColor
    CommonComputationalGeometry
    CommonCore
    CommonDataModel
    CommonExecutionModel
    CommonTransforms
    FiltersCore
    FiltersGeneral
    FiltersHybrid
    FiltersModeling
    FiltersSources
    IOImage
    IOXML
    InteractionStyle
    InteractionWidgets
    RenderingAnnotation
    RenderingCore
    RenderingFreeType
    RenderingLabel
    RenderingOpenGL2
    OPTIONAL_COMPONENTS
    cli11
    fmt
    TestingRendering
    QUIET
    )
endif()

set(KIT_LIBS ${VTK_LIBRARIES})

#
# Build all .cxx files in the directory
file(GLOB ALL_FILES *.cxx)

# set(VERSION_MIN "7.0")
# Requires_Version(ParametricKuenDemo ${VERSION_MIN} ALL_FILES)
# Requires_Version(ParametricObjectsDemo2 ${VERSION_MIN} ALL_FILES)

include(${WikiExamples_SOURCE_DIR}/CMake/RequiresModule.cmake)
Requires_Module(OpenVRCone RenderingOpenVR)
Requires_Module(OpenVRCube RenderingOpenVR)
Requires_Module(OpenVRCylinder RenderingOpenVR)
Requires_Module(OpenVRFrustum RenderingOpenVR)
Requires_Module(OpenVROrientedArrow RenderingOpenVR)
Requires_Module(OpenVROrientedCylinder RenderingOpenVR)
Requires_Module(OpenVRSphere RenderingOpenVR)
Requires_Module(OpenVRTessellatedBoxSource RenderingOpenVR)
Requires_Module(ParametricObjectsDemo cli11)
Requires_Module(ParametricObjectsDemo fmt)

foreach(SOURCE_FILE ${ALL_FILES})
  string(REPLACE ".cxx" "" TMP ${SOURCE_FILE})
  string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "" EXAMPLE ${TMP})
  add_executable(${WIKI}${EXAMPLE} ${EXECUTABLE_FLAG} ${EXAMPLE}.cxx)
  target_link_libraries(${WIKI}${EXAMPLE} ${KIT_LIBS})
  vtk_module_autoinit(
    TARGETS ${WIKI}${EXAMPLE}
    MODULES ${VTK_LIBRARIES}
    )
endforeach()

if (BUILD_TESTING)
  # Testing
  set(KIT GeometricObjects)
  set(NEEDS_ARGS
    QuadraticHexahedron
    TessellatedBoxSource
    )

  add_test(${KIT}-QuadraticHexahedron ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestQuadraticHexahedron -E 30)

  add_test(${KIT}-TessellatedBoxSource ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestTessellatedBoxSource -E 30)

  include(${WikiExamples_SOURCE_DIR}/CMake/ExamplesTesting.cmake)
endif()
