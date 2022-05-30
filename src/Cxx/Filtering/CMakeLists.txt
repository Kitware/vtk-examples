project (${WIKI}Filtering)

if(NOT VTK_BINARY_DIR)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    CommonColor
    CommonCore
    CommonDataModel
    CommonMath
    CommonTransforms
    FiltersCore
    FiltersExtraction
    FiltersGeneral
    FiltersGeneric
    FiltersModeling
    FiltersProgrammable
    FiltersSources
    IOGeometry
    IOPLY
    IOXML
    ImagingHybrid
    ImagingMath
    InteractionStyle
    RenderingCore
    RenderingFreeType
    RenderingOpenGL2
    OPTIONAL_COMPONENTS
    TestingGenericBridge
    TestingRendering
    QUIET
    )
endif()

set(KIT_LIBS ${VTK_LIBRARIES})

#
# Build all .cxx files in the directory
file(GLOB ALL_FILES *.cxx)

if(TestingGenericBridge_LOADED OR TARGET VTK::TestingGenericBridge)
  set(TESTING_GENERIC_BRIDGE TRUE)
endif()

Requires_Setting_On (GenericClip TESTING_GENERIC_BRIDGE)

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
  set(KIT Filtering)
  set(NEEDS_ARGS
    ConnectivityFilterDemo
    ConstrainedDelaunay2D
    ContoursFromPolyData
    ConstrainedDelaunay2D
    ICPRealData
    SurfaceFromUnorganizedPoints
    )

  set(DATA ${WikiExamples_SOURCE_DIR}/src/Testing/Data)

  add_test(${KIT}-ConnectivityFilterDemo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestConnectivityFilterDemo ${DATA}/fsu/stratocaster.ply)

  add_test(${KIT}-ConstrainedDelaunay2D ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestConstrainedDelaunay2D -E 30)

  add_test(${KIT}-Delaunay2D ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestDelaunay2D -E 30)

  add_test(${KIT}-SurfaceFromUnorganizedPoints ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestSurfaceFromUnorganizedPoints ${DATA}/Bunny.vtp -E 30)

  add_test(${KIT}-ContoursFromPolyData ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestContoursFromPolyData ${DATA}/Bunny.vtp  -E 30)
  include(${WikiExamples_SOURCE_DIR}/CMake/ExamplesTesting.cmake)

endif()
