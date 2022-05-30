project (${WIKI}Widgets)

if(NOT VTK_BINARY_DIR)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    CommonColor
    CommonCore
    CommonDataModel
    CommonTransforms
    FiltersCore
    FiltersSources
    GeovisCore
    IOImage
    IOLegacy
    IOXML
    ImagingGeneral
    ImagingHybrid
    ImagingSources
    ImagingStatistics
    ImagingStencil
    InteractionStyle
    InteractionWidgets
    RenderingAnnotation
    RenderingCore
    RenderingFreeType
    RenderingOpenGL2
    OPTIONAL_COMPONENTS
    TestingRendering
    QUIET
    )
endif()

set(KIT_LIBS ${VTK_LIBRARIES})

#
# Build all .cxx files in the directory
file(GLOB ALL_FILES *.cxx)
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
  set(KIT Widgets)

  set(NEEDS_ARGS
    CameraOrientationWidget
    CheckerboardWidget
    OrientationMarkerWidget1
    RectilinearWipeWidget
    ScalarBarWidget
    Slicer2D
    )

  set(DATA ${WikiExamples_SOURCE_DIR}/src/Testing/Data)

  add_test(${KIT}-CameraOrientationWidget ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestCameraOrientationWidget ${DATA}/cow.vtp)

  add_test(${KIT}-CheckerboardWidget ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestCheckerboardWidget ${DATA}/Gourds2.jpg ${DATA}/Ox.jpg)

  add_test(${KIT}-OrientationMarkerWidget1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestOrientationMarkerWidget1 ${DATA}/Bunny.vtp)

  add_test(${KIT}-RectilinearWipeWidget ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRectilinearWipeWidget ${DATA}/Gourds2.jpg ${DATA}/Ox.jpg)

  add_test(${KIT}-ScalarBarWidget ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestScalarBarWidget ${DATA}/uGridEx.vtk )

  add_test(${KIT}-Slider2D ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestSlider2D -E 25)

  include(${WikiExamples_SOURCE_DIR}/CMake/ExamplesTesting.cmake)

endif()
