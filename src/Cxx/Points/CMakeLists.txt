project (${WIKI}Points)

if(NOT VTK_BINARY_DIR)
  set(optional "")
  Add_Optional_If_In_Library(Powercrust "${VTK_AVAILABLE_COMPONENTS}" optional)
  Add_Optional_If_In_Library(PoissonReconstruction "${VTK_AVAILABLE_COMPONENTS}" optional)
message(STATUS "optional: ${optional}")
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    CommonColor
    CommonCore
    CommonDataModel
    CommonSystem
    FiltersCore
    FiltersGeneral
    FiltersPoints
    FiltersSources
    IOGeometry
    IOImage
    IOPLY
    IOXML
    ImagingCore
    InteractionStyle
    RenderingAnnotation
    RenderingCore
    RenderingFreeType
    RenderingOpenGL2
    ${optional}
    OPTIONAL_COMPONENTS
    TestingRendering
    QUIET
    )
endif()
set(KIT_LIBS ${VTK_LIBRARIES})

#
# Build all .cxx files in the directory
file(GLOB ALL_FILES *.cxx)

Requires_Module(PowercrustExtractSurface Powercrust)
Requires_Module(PoissonExtractSurface PoissonReconstruction)
Requires_Module(CompareExtractSurface Powercrust)
Requires_Module(CompareExtractSurface PoissonReconstruction)

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
  set(KIT Points)
  set(NEEDS_ARGS
    CompareExtractSurface
    ExtractEnclosedPoints
    ExtractSurface
    ExtractSurfaceDemo
    DensifyPoints
    MaskPointsFilter
    PointOccupancy
    RadiusOutlierRemoval
    SignedDistance
    UnsignedDistance
    )
  set(DATA ${WikiExamples_SOURCE_DIR}/src/Testing/Data)

  if(TARGET CompareExtractSurface)
    add_test(${KIT}-CompareExtractSurface ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
      TestCompareExtractSurface ${DATA}/horsePoints.vtp -E 40)
  endif()

  if(TARGET ExtractEnclosedPoints)
    add_test(${KIT}-ExtractEnclosedPoints ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
      TestExtractEnclosedPoints ${DATA}/k.vtk)
  endif()

  if(TARGET ExtractSurface)
    add_test(${KIT}-ExtractSurface ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
      TestExtractSurface ${DATA}/Armadillo.ply)
  endif()
  add_test(${KIT}-ExtractSurfaceDemo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestExtractSurfaceDemo ${DATA}/Armadillo.ply)
  add_test(${KIT}-DensifyPoints ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestDensifyPoints ${DATA}/Torso.vtp)
  add_test(${KIT}-MaskPointsFilter ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestMaskPointsFilter ${DATA}/FullHead.mhd)
  add_test(${KIT}-PointOccupancy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestPointOccupancy ${DATA}/cowHead.vtp)
  add_test(${KIT}-RadiusOutlierRemoval ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRadiusOutlierRemoval ${DATA}/cowHead.vtp)
  add_test(${KIT}-SignedDistance ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestSignedDistance ${DATA}/Armadillo.ply)
  add_test(${KIT}-UnsignedDistance ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestUnsignedDistance ${DATA}/Armadillo.ply)

  include(${WikiExamples_SOURCE_DIR}/CMake/ExamplesTesting.cmake)
endif()
