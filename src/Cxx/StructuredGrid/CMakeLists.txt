project (${WIKI}StructuredGrid)

if(NOT VTK_BINARY_DIR)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    CommonColor
    CommonCore
    CommonDataModel
    FiltersCore
    FiltersGeneral
    FiltersGeometry
    FiltersSources
    IOXML
    InteractionStyle
    RenderingCore
    RenderingFreeType
    RenderingOpenGL2
    OPTIONAL_COMPONENTS
    RenderingGL2PSOpenGL2
    TestingRendering
    QUIET
    )
  if (VTK_VERSION VERSION_LESS "8.90.0")
    include(${VTK_USE_FILE})
  endif()

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
  set(KIT StructuredGrid)
  set(NEEDS_ARGS
    )
  include(${WikiExamples_SOURCE_DIR}/CMake/ExamplesTesting.cmake)
endif()
